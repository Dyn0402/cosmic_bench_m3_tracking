#!/bin/bash
# Per-file M3 v2 tracking condor job. One argument: the 0-based job index into
# joblist.txt. Runs entirely in the condor scratch dir, copies inputs from EOS
# to local scratch, and writes the rays.root to <SUBRUN>/m3_tracking_root_v2/ on
# EOS. Idempotent: exits 0 immediately if the output already exists.
# NB: no `set -u` — the LCG view's setup.sh references unset variables and would
# abort the shell at `source` under nounset. Every step is checked explicitly.
IDX="${1:-}"
LCG=/cvmfs/sft.cern.ch/lcg/views/LCG_107/x86_64-el9-gcc13-opt/setup.sh
STAGE=/eos/experiment/ntof/data/x17/cosmic_bench/june_tests/_m3_v2_condor

fail(){ echo "JOB ${IDX} FAILED: $*" >&2; exit 1; }
[ -n "$IDX" ] || fail "no job index argument"

# --- read this job's line (joblist staged with the binaries) ---
[ -f "$STAGE/joblist.txt" ] || fail "no joblist at $STAGE"
LINE="$(sed -n "$((IDX+1))p" "$STAGE/joblist.txt")"
[ -n "$LINE" ] || fail "empty joblist line $IDX"
IFS='|' read -r SUB COSBASE PEDBASE NUM OUTNAME <<< "$LINE"
echo "[$(date)] host=$(hostname) idx=$IDX"
echo "  sub=$SUB"
echo "  cos=$(basename "$COSBASE")  ped=$(basename "$PEDBASE")  num=$NUM  out=$OUTNAME"

OUTDIR="$SUB/m3_tracking_root_v2"
OUTPATH="$OUTDIR/$OUTNAME"
if [ -s "$OUTPATH" ]; then echo "  output exists, skip"; exit 0; fi

WORK="$PWD"                 # condor scratch
mkdir -p "$WORK/root_files"
cd "$WORK" || fail "cd scratch"

source "$LCG" 2>/dev/null || fail "LCG source"

# --- binaries + config from EOS staging ---
cp "$STAGE/DataReader" "$STAGE/tracking" "$STAGE/config_ref_june2026.json" "$WORK/" || fail "copy binaries"
chmod +x "$WORK/DataReader" "$WORK/tracking"

# --- copy input fdfs to local scratch (avoid repeated fuse reads of big files) ---
PED000="${PEDBASE}000_01.fdf"
DATF="${COSBASE}${NUM}_01.fdf"
# aborted runs leave 0-byte fdfs; nothing to track, succeed without output
if [ ! -s "$DATF" ]; then echo "  data file empty/absent ($DATF), skip"; exit 0; fi
if [ ! -s "$PED000" ]; then fail "pedestal empty/absent ($PED000)"; fi
cp "$PED000" "$WORK/ped_000_01.fdf" || fail "copy pedestal $PED000"
cp "$DATF"   "$WORK/dat_${NUM}_01.fdf" || fail "copy data $DATF"
# local basenames so the config's <base><NNN>_01.fdf pattern resolves in scratch
PEDLOCAL="$WORK/ped_"
DATLOCAL="$WORK/dat_"

strip_comments(){ python3 -c "
import sys
for line in sys.stdin:
    o=[];q=False;i=0
    while i<len(line):
        c=line[i]
        if c=='\"': q=not q
        if not q and line[i:i+2]=='//': break
        o.append(c); i+=1
    print(''.join(o).rstrip())
"; }

mkcfg(){ # $1=first $2=last $3=basename $4=outfile
  { cat "$WORK/config_ref_june2026.json";
    echo "\"data_file_first\" : $1,";
    echo "\"data_file_last\" : $2,";
    echo "\"data_file_basename\" : \"$3\"}"; } | strip_comments > "$4"; }

# --- pedestals ---
mkcfg 0 0 "$PEDLOCAL" cfg_ped.json
"$WORK/DataReader" cfg_ped.json read  > read.log  2>&1 || fail "DataReader read (see read.log)"
"$WORK/DataReader" cfg_ped.json ped   > ped.log   2>&1 || fail "DataReader ped"
[ -s root_files/test_Ped.dat ] || fail "no pedestal produced"

# --- analyse + rays for this file ---
mkcfg $((10#$NUM)) $((10#$NUM)) "$DATLOCAL" cfg_cos.json
"$WORK/DataReader" cfg_cos.json analyse > analyse.log 2>&1 || fail "DataReader analyse"
"$WORK/tracking"  cfg_cos.json rays "$WORK/out.root" > track.log 2>&1
tail -1 track.log
[ -s "$WORK/out.root" ] || fail "no rays.root produced (see track.log)"

# --- publish to EOS ---
mkdir -p "$OUTDIR" || fail "mkdir $OUTDIR"
cp "$WORK/out.root" "$OUTPATH.tmp" || fail "cp to EOS"
mv "$OUTPATH.tmp" "$OUTPATH" || fail "mv on EOS"
chmod 666 "$OUTPATH" 2>/dev/null
echo "[$(date)] OK -> $OUTPATH ($(stat -c%s "$OUTPATH" 2>/dev/null) bytes)"
