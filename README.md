# tioj-judge
### TIOJ judge client

Basic structure: **Communicator - Controller - Graders**

## 5-stage grading

1. Pre-execution
   - **Validation** (used when source code is restricted) **and compilation**
   - There can be different pre-execution commands in a problem (e.g., different programming languages)
2. Execution
   - **Run main program(s)**
     - Produce output file(s) [in most cases but not necessary]
   - Collect running status of sandbox, e.g., running time, memory usage, verdict (normal, TLE, MLE, RE)
3. Checking
   - If running status is not 'normal', skip this stage
   - **Arrange output and running status into one or more result fields**
     - 0/1 (WA/AC) in most cases, but additional information may be given here
     - These fields will be sent to the web server
   - (Execution and checking stage are run once per required testdata)
4. Scoring
   - Arrange all running status & results into a single file
   - **Determine overall results of this submission**
5. Post-scoring
   - This stage is optional, used by *Codechef challenge*-like problems only
   - Arrange overall results of all submissions into a single file
   - **Adjust overall results of submissions**

Steps marked in bold allow problem setters customization and should be run in the sandbox.

### Competition Mode

Used by problems involving interaction between submissions.

1. Pre-execution stage
   - Same as ordinary problems
2. Competition stage
   - **Problem setter writes a controller to hold the competition, and output results of all participating programs**
   - A controller will require higher permission (e.g., `fork`, `exec`) to run submissions in the desired way
     - Non-admin can't set this type of problems

The whole competition is held again once a new program is submitted
- Need stricter restrictions on submissions

### Grader

A grader process handles a part of submission grading only.

The grading process is separated into:

- Pre-execution stage
- Execution & checking stage (combined) of each testdata
- Scoring stage

Graders are run without root permission. The permission required by the sandbox is obtained from setuid.

## Communicator & API

?

(fetch new submissions, sync testdata, problem settings and return results)

(need adapter to be backward compatible)

## Controller

Control grading processes of queueing submissions
- Manage CPU threads
- Determine which testdata to run
  - It should support incomplete grading (if the submission doesn't pass a testdata, the remaining testdata in the same subtask are not graded)
- Create grading environment for graders (directories, sources, testdata, etc.)
- Handle abnormal termination

## Sandbox

### Requirements

- VSS / RSS / running time measurement & limit
- Restricted system call
  - Basic file operations (e.g., `open`, `close`, `lseek`) should be allowed optionally
    - (but only in the current directory?)
- Basic redirection / pipe operations
  - Cross-bound stdin & stdout (CF-like interactive)
- Run multiple commands (or just run sandbox several times?)
- Output limit (?)
