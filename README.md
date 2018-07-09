# tioj-judge
### TIOJ judge client

**Detail update needed!**

Basic process structure: **Communicator === Scheduler - Graders - Sandbox**

## 4-stage grading

1. Pre-execution
   - **Validation** (used when source code is restricted) **and compilation**
   - There can be different pre-execution commands in a problem (e.g., different programming languages)
2. Execution
   - **Run main program(s)**
     - Produce output file(s) [in most cases but not necessary]
   - Collect running status of sandbox, e.g., running time, memory usage, verdict (normal, TLE, MLE, RE)
3. Evaluation
   - **Arrange output and running status into one or more result fields**
     - 0/1 (WA/AC) in most cases, but additional information may be given here
     - Some fields would be sent to the web server
   - (Execution and evaluation stage are run once per required testdata)
4. Scoring
   - Arrange all running status & results into a single file
   - **Determine overall results of all submissions** (if necessary)

Several default routines are provided. Customization of any stage is allowed (admin only).

### 1-stage grading

Fully-customized mode. Problem setter writes a program to handle the whole judging process.

Since a controller will require higher permission (e.g., `fork` or user changing) to run submissions in the desired way, non-admin can't set this type of problems

### Grader

A grader process handles a part of submission grading only. It will setup everything inside directories of its task (dynamic libs, files, fds, pipes) and call the sandbox to execute program(s).

The grading process is separated into:

- Pre-execution stage
- Execution & checking stage (separated) of each testdata
- Scoring stage

## Communicator

### Core

Handles all communication with web server.
(fetch new submissions, sync testdata, problem settings and return results)
(API needed)

### Main

- Gets (kill / new submission / rejudge) requests from core
- Maintains problem settings information and (directory / database) structures
- Sends requests scheduler, and collect results from it

## Scheduler

Control grading processes of queueing submissions
- Manage CPU threads and box dirs
- Determine which testdata to run
  - It should support incomplete grading (if the submission doesn't pass a testdata, the remaining testdata in the same subtask are not graded)
- Handle abnormal termination

### Directory Structure

**update needed**
