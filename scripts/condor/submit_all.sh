#!/bin/bash
# Driver for the condor M3 v2 reprocessing (run on an lxplus login node).
#
#   ./submit_all.sh            # build joblist, stage, submit all files
#   ./submit_all.sh -n         # dry run: build joblist + stage, do NOT submit
#
# Prereqs: a valid kerberos ticket (klist), the v2 binaries built at
# $BINREPO (see scripts/../README build steps), and write access to the
# june_tests EOS tree.
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
BINREPO="${BINREPO:-$HOME/m3_tracking_v2}"      # where DataReader/tracking are built
STAGE=/eos/experiment/ntof/data/x17/cosmic_bench/june_tests/_m3_v2_condor
WORK="${WORK:-$HOME/m3_condor}"
DRY=""; [ "${1:-}" = "-n" ] && DRY=1

# --- kerberos preflight -------------------------------------------------------
# The whole run needs a valid ticket: condor forwards it to the batch nodes for
# EOS writes, AND the schedd writes each job's .out/.err/.log back to the AFS
# submit dir. If the ticket lapses mid-run, late jobs go held and are removed
# (this bit us once: a 7 h default ticket expired ~4 h into a 275-job run).
# Get a long-lived renewable ticket BEFORE submitting so it outlives the run.
if ! klist -s 2>/dev/null; then
  echo "!! no valid kerberos ticket. Run:  kinit -r 7d ${USER}@CERN.CH" >&2; exit 1
fi
aklog 2>/dev/null || true                             # kerberos TGT -> AFS token
kinit -R 2>/dev/null || true                          # renew if renewable (best effort)
# best-effort lifetime warning (klist date formats vary; never block on a parse miss)
EXP="$(klist 2>/dev/null | awk '/krbtgt/{print $3, $4; exit}')"
ES="$(date -d "$EXP" +%s 2>/dev/null || echo 0)"
if [ "$ES" -gt 0 ]; then
  LEFT=$(( ES - $(date +%s) ))
  if [ "$LEFT" -lt 21600 ]; then
    echo "!! ticket has < 6 h left. A 275-file run can take hours; renew first:" >&2
    echo "   kinit -r 7d ${USER}@CERN.CH   (then re-run; submit is idempotent)" >&2
    exit 1
  fi
fi

mkdir -p "$WORK/logs"
cd "$WORK"
cp "$HERE/gen_joblist.sh" "$HERE/m3_job.sh" "$HERE/m3.sub" .
chmod +x gen_joblist.sh m3_job.sh

echo ">> building joblist"
bash gen_joblist.sh joblist.txt joblist.skipped
NJOBS=$(wc -l < joblist.txt)

echo ">> staging binaries + config + joblist + wrapper to EOS"
mkdir -p "$STAGE"
cp "$BINREPO/DataReader" "$BINREPO/tracking" "$BINREPO/config_ref_june2026.json" \
   joblist.txt m3_job.sh "$STAGE/"

if [ -n "$DRY" ]; then
  echo ">> dry run: $NJOBS jobs staged, not submitting"; exit 0
fi
echo ">> submitting $NJOBS jobs"
condor_submit m3.sub -queue "$NJOBS"
echo ">> submitted. Track with: $HERE/monitor.sh"
