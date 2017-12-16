# tioj-judge
### TIOJ judge client

Basic structure: **Communicator - Controller - Graders**

## 4-stage grading

1. Pre-execution
   - **Validation** (used when source code is restricted) **and compilation**
2. Execution
   - **Run main program(s) in sandbox**
   - Usually produce output file(s)
   - Determine running status of sandbox, e.g. running time, memory usage, verdict (normal, TLE, MLE, RE)
3. Checking
   - If running status is not 'normal', skip this stage
   - **Arrange output and running status into one or more result fields**
     - Usually it is just 0/1 (WA/AC), but additional information may be given here
     - These fields will be sent to the web server
   - (Execution and checking stage are runned once per required testdata)
4. Scoring
   - Arrange all running status & results into a single file
   - **Determine overall results of this submission**
