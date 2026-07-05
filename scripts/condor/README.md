# M3 v2 tracking on HTCondor (lxplus)

Reprocess every June cosmic-bench M3 datrun file into
`<subrun>/m3_tracking_root_v2/*_rays.root`, one condor job per file, using the
v2 tracking (`config_ref_june2026.json`, NClus branches, charge-weighted
centroid, layer-drop rescue, ZS fix). Originals in `m3_tracking_root/` are
never touched.

## Prerequisites (once)

```bash
ssh lxplus
git clone https://github.com/Dyn0402/cosmic_bench_m3_tracking.git ~/m3_tracking_v2
cd ~/m3_tracking_v2
source /cvmfs/sft.cern.ch/lcg/views/LCG_107/x86_64-el9-gcc13-opt/setup.sh
make cleanall && make tracking DataReader -j4
kinit    # ensure a valid kerberos ticket (klist to check)
```

## Run

```bash
cd ~/m3_tracking_v2/scripts/condor
./submit_all.sh -n     # dry run: build+stage joblist, print job count
./submit_all.sh        # stage and submit all files
./monitor.sh -w        # watch until the queue drains
```

`submit_all.sh` builds `~/m3_condor/joblist.txt` (one line per datrun file),
stages the binaries + config + joblist + wrapper to
`.../june_tests/_m3_v2_condor/` on EOS, and submits `m3.sub` with `-queue N`.

## Design notes

- **One job per datrun file** (`m3_job.sh <index>`), reads its line from the
  staged `joblist.txt`. Fully self-contained: builds its own pedestal, so
  there are no inter-job dependencies and any job can be re-run alone.
- **Idempotent**: a job whose output already exists exits 0 immediately, so
  re-submitting only fills gaps (failed/evicted jobs, newly added runs).
- **EOS access from batch nodes** works over the fuse mount with a forwarded
  kerberos credential (`MY.SendCredential = true`). Inputs are copied to the
  job's local scratch first to avoid repeated fuse reads of the big fdfs.
- **Empty inputs** (0-byte aborted-run fdfs) are skipped cleanly (exit 0).
- **Throttling**: `max_materialize = 60` caps how many jobs are live at once,
  so EOS never sees hundreds of concurrent multi-hundred-MB reads.
- **Retries**: a job that exits non-zero is retried up to 3 starts
  (`periodic_remove = NumJobStarts > 3`); logs land in `~/m3_condor/logs/`.

## Validate before switching analyses to v2

```bash
# same evn set as the originals, NClus branches present, more events-with-ray
./monitor.sh          # outputs=N/M when done
```
Then spot-check a run (see mx_june_cosmic_qa/HANDOFF_m3_v2_reprocessing.md):
identical `evn` set to `m3_tracking_root/`, `NClusX`/`NClusY` present, and use
the consumer recipe `NClusX>=3 & NClusY>=3 & Chi2X+Chi2Y<5`.
