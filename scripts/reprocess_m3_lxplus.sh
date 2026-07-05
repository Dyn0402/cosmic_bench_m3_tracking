#!/bin/bash
# Reprocess M3 reference tracking (v2) for the June cosmic-bench runs on lxplus.
#
# Usage:
#   ./scripts/reprocess_m3_lxplus.sh [subrun-filter]
#
#   subrun-filter: optional grep pattern applied to <run>/<subrun> paths, so
#   several lxplus sessions can split the work, e.g.
#     ./scripts/reprocess_m3_lxplus.sh 'det3_saturday'
#     ./scripts/reprocess_m3_lxplus.sh 'det1_det2|det2_det3'
#
# Environment: source an LCG view with ROOT + gcc + boost first, e.g.
#   source /cvmfs/sft.cern.ch/lcg/views/LCG_107/x86_64-el9-gcc13-opt/setup.sh
# and build with `make cleanall; make tracking DataReader -j4` from the repo root.
#
# Outputs go to <subrun>/m3_tracking_root_v2/ on EOS (the original
# m3_tracking_root/ is never touched). Existing v2 outputs are skipped, so the
# script is restartable. Subruns without a *_pedthr_*_01.fdf are reported and
# skipped -- resolve those by pointing PEDOVERRIDE at a pedestal basename.

set -u
BASE=/eos/experiment/ntof/data/x17/cosmic_bench/june_tests
REPO="$(cd "$(dirname "$0")/.." && pwd)"
FILTER="${1:-.}"
FEU=01           # M3 FEU file suffix
PEDOVERRIDE=""   # optional: full path prefix of a pedestal fdf set (up to _NNN_01.fdf)

WORK="${TMPDIR:-/tmp}/m3_reproc_$$"
mkdir -p "$WORK/root_files"
cd "$WORK"
cp "$REPO/config_ref_june2026.json" .

strip_comments() {
python3 -c "
import sys
for line in sys.stdin:
    out=[]; instr=False; i=0
    while i < len(line):
        c=line[i]
        if c=='\"': instr = not instr
        if not instr and line[i:i+2]=='//': break
        out.append(c); i+=1
    print(''.join(out).rstrip())
"
}

make_config() {  # $1=first $2=last $3=basename $4=outfile
  { cat "$REPO/config_ref_june2026.json";
    echo "\"data_file_first\" : $1,";
    echo "\"data_file_last\" : $2,";
    echo "\"data_file_basename\" : \"$3\"}"; } | strip_comments > "$4"
}

SKIPPED=()
shopt -s nullglob
for RAW in "$BASE"/*/*/raw_daq_data; do
  SUB="$(dirname "$RAW")"
  TAG="${SUB#$BASE/}"
  echo "$TAG" | grep -qE "$FILTER" || continue

  DATFILES=("$RAW"/*_datrun_*_000_"$FEU".fdf)
  [ ${#DATFILES[@]} -eq 0 ] && continue
  OUT="$SUB/m3_tracking_root_v2"
  mkdir -p "$OUT"
  echo "=== $TAG"

  # a subrun can hold several datrun sets (e.g. an aborted start); handle each
  for DAT000 in "${DATFILES[@]}"; do
    COSBASE="${DAT000%000_${FEU}.fdf}"             # .../<name>_datrun_<stamp>_
    # pedestal: prefer the pedthr matching this datrun's timestamp, else any
    # pedthr in the subrun, else the override, else skip
    PEDMATCH="${COSBASE/_datrun_/_pedthr_}"
    PEDFILES=("$RAW"/*_pedthr_*_000_"$FEU".fdf)
    if [ -s "${PEDMATCH}000_${FEU}.fdf" ]; then
      PEDBASE="$PEDMATCH"
    elif [ ${#PEDFILES[@]} -gt 0 ]; then
      PEDBASE="${PEDFILES[0]%000_${FEU}.fdf}"
    elif [ -n "$PEDOVERRIDE" ]; then
      PEDBASE="$PEDOVERRIDE"
    else
      echo "SKIP (no pedthr): $TAG $(basename "$COSBASE")"; SKIPPED+=("$TAG $(basename "$COSBASE")"); continue
    fi

    # pedestals (once per datrun set)
    make_config 0 0 "$PEDBASE" config_ped.json
    rm -f root_files/test_signal.root root_files/test_RMSPed.dat root_files/test_Ped.dat
    "$REPO/DataReader" config_ped.json read > /dev/null 2>&1
    "$REPO/DataReader" config_ped.json ped  > /dev/null 2>&1
    if [ ! -s root_files/test_Ped.dat ]; then
      echo "PED FAILED: $TAG $(basename "$PEDBASE")"; SKIPPED+=("$TAG (ped failed)"); continue
    fi

    for F in "${COSBASE}"[0-9][0-9][0-9]_"$FEU".fdf; do
      NUM="$(basename "$F" | sed -E "s/.*_([0-9]{3})_${FEU}\.fdf/\1/")"
      NAME="$(basename "${COSBASE%_}")_${NUM}_rays.root"
      if [ -s "$OUT/$NAME" ]; then echo "  $NAME: exists, skip"; continue; fi
      make_config $((10#$NUM)) $((10#$NUM)) "$COSBASE" config_cos.json
      rm -f root_files/test_analyse.root
      "$REPO/DataReader" config_cos.json analyse > /dev/null 2>&1
      "$REPO/tracking"  config_cos.json rays "$OUT/$NAME" > "$WORK/track_${NUM}.log" 2>&1
      if [ -s "$OUT/$NAME" ]; then
        chmod 666 "$OUT/$NAME" 2>/dev/null
        echo "  $NAME: OK"
      else
        echo "  $NAME: FAILED (see $WORK/track_${NUM}.log)"
      fi
    done
  done
done

echo; echo "Done. Skipped subruns:"; printf '  %s\n' "${SKIPPED[@]:-none}"
