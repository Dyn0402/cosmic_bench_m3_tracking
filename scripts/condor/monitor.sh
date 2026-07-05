#!/bin/bash
# Progress of the M3 v2 condor reprocessing. Run on lxplus.
#   ./monitor.sh          # one snapshot
#   ./monitor.sh -w       # refresh every 30 s until the queue drains
BASE=/eos/experiment/ntof/data/x17/cosmic_bench/june_tests
WORK="${WORK:-$HOME/m3_condor}"
snap(){
  local njobs done inq held
  njobs=$(wc -l < "$WORK/joblist.txt" 2>/dev/null || echo '?')
  done=$(find "$BASE" -path "*/m3_tracking_root_v2/*_rays.root" 2>/dev/null | wc -l)
  inq=$(condor_q "$@" -totals -af:r JobStatus 2>/dev/null | wc -l)   # unused fallback
  inq=$(condor_q "$@" -nobatch 2>/dev/null | grep -cE '^ *[0-9]+\.[0-9]+')
  held=$(condor_q "$@" -nobatch 2>/dev/null | grep -c ' H ')
  echo "$(date +%H:%M:%S)  outputs=$done/$njobs   in_queue=$inq   held=$held"
  if [ "$held" -gt 0 ]; then
    condor_q "$@" -held -af HoldReason 2>/dev/null | sort | uniq -c | head -5 | sed 's/^/    HELD: /'
  fi
}
CL="${2:-}"    # optional cluster id as 2nd arg to scope to our submission
if [ "${1:-}" = "-w" ]; then
  while :; do
    snap $CL
    [ "$(condor_q $CL -nobatch 2>/dev/null | grep -cE '^ *[0-9]+\.[0-9]+')" -eq 0 ] && { echo "queue drained"; break; }
    sleep 30
  done
else snap "${1:-}"; fi
