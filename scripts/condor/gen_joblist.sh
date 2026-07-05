#!/bin/bash
# Enumerate one condor job per M3 datrun file across june_tests.
# Emits joblist.txt lines: SUBRUN_DIR|COSBASE|PEDBASE|FILENUM|OUTNAME
# (| chosen as delimiter: never appears in these EOS paths).
# Subruns/datrun-sets with no resolvable pedestal are written to joblist.skipped.
set -u
BASE=/eos/experiment/ntof/data/x17/cosmic_bench/june_tests
FEU=01
OUT=${1:-joblist.txt}
SKIP=${2:-joblist.skipped}
: > "$OUT"; : > "$SKIP"
shopt -s nullglob

for RAW in "$BASE"/*/*/raw_daq_data; do
  SUB="$(dirname "$RAW")"
  DATFILES=("$RAW"/*_datrun_*_000_"$FEU".fdf)
  [ ${#DATFILES[@]} -eq 0 ] && continue
  for DAT000 in "${DATFILES[@]}"; do
    COSBASE="${DAT000%000_${FEU}.fdf}"
    PEDMATCH="${COSBASE/_datrun_/_pedthr_}"
    PEDFILES=("$RAW"/*_pedthr_*_000_"$FEU".fdf)
    if [ -s "${PEDMATCH}000_${FEU}.fdf" ]; then
      PEDBASE="$PEDMATCH"
    elif [ ${#PEDFILES[@]} -gt 0 ]; then
      PEDBASE="${PEDFILES[0]%000_${FEU}.fdf}"
    else
      echo "$SUB | $(basename "$COSBASE")" >> "$SKIP"; continue
    fi
    for F in "${COSBASE}"[0-9][0-9][0-9]_"$FEU".fdf; do
      NUM="$(basename "$F" | sed -E "s/.*_([0-9]{3})_${FEU}\.fdf/\1/")"
      OUTNAME="$(basename "${COSBASE%_}")_${NUM}_rays.root"
      echo "${SUB}|${COSBASE}|${PEDBASE}|${NUM}|${OUTNAME}" >> "$OUT"
    done
  done
done
echo "jobs: $(wc -l < "$OUT")   skipped datrun-sets: $(wc -l < "$SKIP")"
