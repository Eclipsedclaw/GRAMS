# GRAMS toy model output

Every run (every `/run/beamOn`) writes its own files into `/OutputDirectory`:

| File | Written when | Content |
|---|---|---|
| `<OutputFile>.root` | `/OutputFormat 1` (default) | the hits, as ROOT trees |
| `<OutputFile>.dat` | `/OutputFormat 0` | the same hits as tab-separated text (the old format) |
| `<OutputFile>_geometry.txt` | `/update` | geometry and primary particle settings |

The output directory is created if it does not exist yet. Before, a missing directory meant that
nothing was written.

## Choosing the output format

```
/OutputFormat 1    # ROOT file <OutputFile>.root (default)
/OutputFormat 0    # text file <OutputFile>.dat, same as before
/OutputFormat -1   # no hit output (any value other than 0 or 1)
```

Both formats contain exactly the same rows: the same hits, in the same order, with the same units.
Macros that set `/OutputFormat 0` keep writing `.dat` files. To switch a macro to ROOT output, change
that line to `/OutputFormat 1`.

## What is in the ROOT file

A `.root` file has two trees: `hits` (the data) and `run` (one entry with the run settings).

### `hits` tree

One entry per recorded step, which is one line of the `.dat` file. The branches are in the same order
as the `.dat` columns. The last column gives the column name used by the analysis code in
`analysis/2023_Paper/toy_model_analysis.py`.

| Branch | Type | Unit | Meaning | `.dat` column name |
|---|---|---|---|---|
| `eventID` | int | | event number, starts at 0 in every file | `eventID` |
| `trackID` | int | | Geant4 track ID, 1 is the primary | `trackID` |
| `parentID` | int | | track ID of the parent, 0 for the primary | `parentID` |
| `particleID` | int | | PDG code, e.g. -2212 anti-proton, 211 pi+ | `particleID` |
| `copyNb1` | int | | copy number of the mother volume | `copyNb1 (mother volume)` |
| `copyNb` | int | | copy number of the volume, see [copy numbers](#copy-numbers) | `copyNb` |
| `material` | string | | material of the volume (`LAr`, `Plastic`, `Fe`, `Vacuum`, ...) | `material` |
| `time` | float | ns | global time at the end of the step | `time (ns)` |
| `energy` | float | MeV | kinetic energy at the start of the step | `energy (MeV)` |
| `eDep` | float | MeV | energy deposited in the step | `energy deposit (MeV)` |
| `parentProcess` | string | | process that created the track, `N/A` for the primary | `parent process name` |
| `processName` | string | | process that ended the step | `process name` |
| `px`, `py`, `pz` | float | MeV/c | momentum at the end of the step | `px (MeV/c)`, ... |
| `stepLength` | float | cm | step length | `step length (cm)` |
| `x`, `y`, `z` | float | cm | position at the end of the step | `x (cm)`, ... |

With `/OutputType 1` (simple mode) `parentProcess` and `processName` are the string `"0"`.

### `run` tree

One entry per file.

| Branch | Type | Unit | Meaning |
|---|---|---|---|
| `nEvents` | int | | number of generated events. The `hits` tree only has the events that were written, so normalize with this number |
| `nStop` | int | | events where the primary stopped in the LAr (the `Stop Event =` line at the end of the run) |
| `nInflight` | int | | events where the primary annihilated or interacted in flight in the LAr |
| `Emin`, `Emax` | double | MeV | `/PrimaryParticle/Emin` and `/PrimaryParticle/Emax` |
| `area` | double | cm | `/PrimaryParticle/area`, side length of the square where primaries start |

`nStop` and `nInflight` are only counted when the primary's steps are recorded (`/TrackType` 0, 1 or
3). With `/StopEvent 1` the same two numbers are still appended to `NoS.txt` as before.

### Copy numbers

For the full geometry (`/DetectorType 1`). Positions are for `/Gondola_Angle 0`, with primaries
coming from +z.

| `copyNb` | Volume | Material |
|---|---|---|
| 0, 1, 2, ... | LAr TPC cell, numbered (layer &times; `NbOfCellL` + column) &times; `NbOfCellL` + cell. `copyNb1` is the TPC number | LAr |
| -1 | chamber (cryostat wall) around the TPC | Fe |
| -10000 | outer TOF, top panel | Plastic |
| -10001 | outer TOF, bottom panel | Plastic |
| -10002, -10003, -10004, -10005 | outer TOF, side panels at -y, +y, -x, +x | Plastic |
| -11000 | inner TOF, top panel | Plastic |
| -11001 | inner TOF, bottom panel | Plastic |
| -11002, -11003, -11004, -11005 | inner TOF, side panels at +y, -y, +x, -x | Plastic |
| -12000 | scoring plane at the top of the atmosphere | Vacuum |
| -12001 | reference plane (`/DetectorType 0` only) | Vacuum |

### Which steps are written

The same macro commands as before decide which steps become rows (see also `macro/README.mac`):

| Command | Values |
|---|---|
| `/TrackType` | 0 all tracks, 1 primaries only, 2 daughters of the primary only (parentID = 1), 3 primary and its daughters |
| `/TrackEdep` | 0 all steps, 1 skip steps without energy deposit that end at a volume boundary |
| `/StopEvent` | 0 all events, 1 only events where the primary stops in the LAr, 2 stop and in-flight events, 3 like 1 but only the earliest TOF and scoring-plane hits plus the steps where a particle stops, 4 currently the same as 2 |
| `/OutputType` | 0 process names written, 1 process names written as `"0"` |

## Reading the output

### Python, with the existing analysis code

`toy_model_analysis.py` has a reader that gives the same pandas DataFrame as the old
`pd.read_csv(..., names=column_names)` on a `.dat` file: same column names, same order. Code written
for the `.dat` files should work on it unchanged (the column types differ slightly, see
[Moving from `.dat` to `.root`](#moving-from-dat-to-root)).

```python
import toy_model_analysis

path = "output/anti_proton/GRASP_anti_proton_1e7_1.root"
df = toy_model_analysis.read_root_from_toymodel(path)
grouped_data = df.groupby(['eventID'])   # as before

# only some columns, which uses much less memory for big files
df = toy_model_analysis.read_root_from_toymodel(path, branches=['eventID', 'trackID', 'energy', 'material'])

# number of generated events, for acceptance and flux
n_generated = toy_model_analysis.read_run_info_from_toymodel(path)['nEvents']
```

This needs [uproot](https://uproot.readthedocs.io): `pip install uproot`. The ROOT installation itself
is not needed for reading.

### Python, with uproot directly

```python
import uproot

with uproot.open("GRASP_anti_proton_1e7_1.root") as f:
    hits = f["hits"].arrays(["eventID", "trackID", "eDep", "material"], library="np")  # dict of numpy arrays
    n_generated = f["run"]["nEvents"].array(library="np")[0]

# files too big for memory: read them in pieces
for chunk in uproot.iterate("GRASP_anti_proton_1e7_*.root:hits", ["eventID", "eDep"], step_size="100 MB", library="np"):
    ...  # chunk["eDep"] is a numpy array
```

Use `library="np"` when you read string branches (`material`, `parentProcess`, `processName`).
`library="pd"` only works for them if the extra package `awkward-pandas` is installed.

### ROOT

```
root -l output/anti_proton/GRASP_anti_proton_1e7_1.root
root [1] hits->Print()
root [2] hits->Draw("energy", "trackID==1 && material==\"LAr\"")
root [3] hits->Scan("eventID:trackID:particleID:material:eDep", "copyNb==-10000")
root [4] run->Scan()
```

All energy bins together with a `TChain`, and with `RDataFrame`:

```cpp
TChain hits("hits");
hits.Add("output/anti_proton/GRASP_anti_proton_1e7_*.root");
hits.Draw("energy", "trackID==1 && material==\"LAr\"");

ROOT::RDataFrame runs("run", "output/anti_proton/GRASP_anti_proton_1e7_*.root");
int nGenerated = *runs.Sum<int>("nEvents");   // generated events in all files

ROOT::RDataFrame df("hits", "output/anti_proton/GRASP_anti_proton_1e7_1.root");
auto eDepLAr = df.Filter("material == \"LAr\"").Sum<float>("eDep");
```

When you combine files, remember that `eventID` restarts at 0 in every file. Use the file (or energy
bin) together with `eventID` to identify an event.

## Running large simulations

### Memory use is now constant

Before this change the memory of a job grew by about 0.5 kB for every event, whatever the output
format: about 5 GB for a run of 1e7 events, which is why jobs had to be split into many macros.
The cause was a bug in `GRAMSDetectorSD::Initialize()`: the hits-collection ID was a `static`
variable shared by the three sensitive detectors, so in every event two of the three hits collections
were never deleted. The ID is now kept per detector.

Peak memory measured for the `anti_proton_1.mac` settings (Apple M4, Geant4 11.4.3, ROOT 6.40):

| Events | Before | After (`.root` output) |
|---|---|---|
| 20 000 | 239 MB | 289 MB |
| 200 000 | 330 MB | 288 MB |
| 1 000 000 | 707 MB | 297 MB |

With `.dat` output the job uses about 50 MB less (240 MB at 20 000 and 241 MB at 200 000 events):
ROOT costs a fixed ~50 MB at start-up for its own initialization, which does not grow with the number
of events. `top` shows up to ~190 MB more for a ROOT job, because it also counts the ROOT libraries
mapped from disk. The `.root` files are about 3 times smaller than the same `.dat` files.

### All energy bins in one job

`macro/GRASP_anti_proton/anti_proton_all.mac` replaces `anti_proton_1.mac` ... `anti_proton_15.mac`.
It runs the 15 bins of 50 MeV (0 to 750 MeV, 1e7 events each) one after the other in one process.
Each bin is its own run and writes its own file, with the same names as before:
`GRASP_anti_proton_1e7_1.root` ... `GRASP_anti_proton_1e7_15.root`.

```bash
./GRAMS macro/GRASP_anti_proton/anti_proton_all.mac
```

The bins are run by `anti_proton_bin.mac` through `/control/loop`. For another particle or other bins,
copy both files and change the particle, `binWidth`, `nEvents`, `fileName` and the loop range
`ibin 1 15 1`. New macro files only reach the build directory when cmake runs, so run `cmake .` in the
build directory after adding them.

Each bin sets its own random seed (`/random/setSeeds {ibin} 12345`), so every bin can also be re-run
on its own with the same result.

### Parallel jobs and random seeds

Running one job per energy bin at the same time is faster when you have several cores, and the memory
of each job now stays constant (about 300 MB in the test above). Every job must use a different random
seed:

```
/random/setSeeds 7 12345    # e.g. the bin number and any fixed number
```

`/gun/seed` does nothing: its value is never given to the random number generator. Without
`/random/setSeeds` every job starts from the same seed, so two jobs with the same settings (for
example one big run split into identical pieces) produce exactly the same events.

### If a job is killed

The ROOT file is saved to disk every 5 minutes while the run goes on. If a job is killed (time limit,
Ctrl-C, crash), the `.root` file can still be opened and has all hits up to the last save. The `run`
tree is only written at the end of the run, so it is missing in that case.

### Runs larger than 100 GB

ROOT starts a new file when one file reaches 100 GB: the hits continue in `<OutputFile>_1.root`,
`<OutputFile>_2.root`, ..., and the `run` tree is in the last file. The end of the log says how many
files were written. Read them together, for example with
`uproot.iterate("<OutputFile>*.root:hits", ...)` or a `TChain`.

### Things that still use memory

Visualization keeps events in memory (`/vis/scene/endOfEventAction accumulate`), so don't use a
visualization macro such as `vis.mac` for large runs.

## Moving from `.dat` to `.root`

- The values are the same. Floating point branches are 32-bit floats: about 7 significant digits,
  more than the 6 digits printed in the `.dat` file.
- Integers are 32-bit (`int32`), floats are `float32` in numpy and pandas, instead of the 64-bit
  types that `pd.read_csv` produces.
- String branches stay strings. In simple mode (`/OutputType 1`) the process names are the string
  `"0"`, where `pd.read_csv` turned them into the number 0.
- `toy_model_analysis.read_data_from_toymodel()` and the h5 helpers still work for `.dat` files.

## Building

ROOT is needed when cmake runs. Set up ROOT and Geant4 first, then build as described in the main
[README](../README.md):

```bash
source /path/to/root/bin/thisroot.sh
source /path/to/geant4/bin/geant4.sh
cmake ../GRAMS
make
```

If cmake stops with `ROOT not found`, ROOT was not set up in that terminal. Source `thisroot.sh` and
run cmake again in a clean build directory.
