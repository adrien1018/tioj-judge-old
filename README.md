# tioj-judge
### TIOJ judge client

**Detail update needed!**

Basic structure: **Communicator - Controller - Graders**

## 5-stage grading

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
     - These fields will be sent to the web server
   - (Execution and evaluation stage are run once per required testdata)
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
   - A controller will require higher permission (e.g., `fork`) to run submissions in the desired way
     - Non-admin can't set this type of problems

The whole competition is held again once a new program is submitted
- Need stricter restrictions on submissions

### Grader

A grader process handles a part of submission grading only. It will setup everything inside directories of its task (dynamic libs, files, fds, pipes) and call the sandbox to execute program(s). 

The grading process is separated into:

- Pre-execution stage
- Execution & checking stage (separated) of each testdata
- Scoring stage

## Communicator & API

?

(fetch new submissions, sync testdata, problem settings and return results)

(need adapter to be backward compatible)

## Controller

Control grading processes of queueing submissions
- Manage CPU threads and box dirs
- Determine which testdata to run
  - It should support incomplete grading (if the submission doesn't pass a testdata, the remaining testdata in the same subtask are not graded)
- Handle abnormal termination

### Directory Structure

```
|- kBoxPath
||- (${SUBDIR}) (for each running submission)
|||- user/ (submission code file(s), ${USRDIR})
|||- assets/ (all files provided by problem setters, ${ASDIR})
|||- temp/ (temporary files if needed, ${TMPDIR})
|||- pre_exec/ (${STAGEDIR})
||||- 00/ (sandbox for 1st program)
||||- ...
|||- testdata000/ (${TDDIR})
||||- exec/ (${STAGEDIR})
|||||- 00/
|||||- ...
||||- eval/ (${STAGEDIR})
|||||- 00/
|||||- ...
|||- ...
|||- score/ (${STAGEDIR})
||||- 00/
||||- ...
|||- post_score/ (${STAGEDIR})
||||- 00/
||||- ...

|- kDataPath
||- 1001/ (normal problem)
|||- metafile
|||- attachments/
|||- testdata/
||||- 000.in (${INPUT})
||||- 000.out (${OUTPUT})
||||- ...
|||- results/ (results of every submission)
||- ...
||- 3001/ (competition problem)
|||- metafile
|||- attachments/
|||- submissions/
||||- metafile (list results)
||||- 123456/
|||||- metafile (language)
|||||- code
|||||- ...
||||- ...
||- ...
```
