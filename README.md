# tioj-judge
### TIOJ judge client

Basic structure: **Communicator - Controller - Graders**

## 4-stage grading

1. Pre-execution
   - **Validation** (used when source code is restricted) **and compilation**
2. Execution
   - **Run main program(s) in sandbox**
     - Produce output file(s) [in most cases but not necessary]
   - Collect running status of sandbox, e.g. running time, memory usage, verdict (normal, TLE, MLE, RE)
3. Checking
   - If running status is not 'normal', skip this stage
   - **Arrange output and running status into one or more result fields**
     - 0/1 (WA/AC) in most cases, but additional information may be given here
     - These fields will be sent to the web server
   - (Execution and checking stage are run once per required testdata)
4. Scoring
   - Arrange all running status & results into a single file
   - **Determine overall results of this submission**

### Grader

A grader process handles a part of submission grading only.

The grading process is separated into:

- Pre-execution stage
- Execution & checking stage of each testdata
- Scoring stage

Graders are run without root permission. The permission required by the sandbox is obtained from setuid.

## Communicator & API

?
(fetch new submissions, sync testdata and problem settings, return results)

## Controller

Control grading processes of queueing submissions
- Manage CPU threads
- Determine which testdata to run
  - It should support incomplete grading (if the submission doesn't pass a testdata, the remaining testdata in the same subtask are not graded)
- Create grading environment for graders (directories, sources, testdata, etc.)
- Handle abnormal termination
