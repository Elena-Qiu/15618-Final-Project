# Parallel Four Color Map Solver (15618 Final Project)

Contributors:
Chenfei Lou (chenfeil@andrew.cmu.edu)
Yuqing Qiu (yuqingqi@andrew.cmu.edu)

Website Link: https://alexanderia-mike.github.io

## Running Instructions
Please setup server and frontend separately as follows.
### Server

```bash
cd server
make server
./server
```
It receives map from frontend, solves it and sends the solution back.
### Frontend
```bash
cd frontend
# download p5.js library
wget https://github.com/processing/p5.js/releases/download/v1.6.0/p5.zip
unzip p5.zip
rm p5.zip
```
Set `IP` and `PORT` of the server in `fourcolors.js`. Then run it on local host. You can use the `Live Server` extension in VS Code.
As shown in the demo below, you can either load preset map or draw your own map for solving.

![demo](demo.gif)

## Testing
For parallel running, needs to install `openMP` library.
### Phase 1: Convert Map to Graph
```bash
cd server
make conversion
```
Then run `./conversion [testcaseFilePath] [sequentialOrNot(true or false)]`. For example, you can run
```bash
./conversion testcases_conversion/1000x1000_0_0_inputs.txt false
```
### Phase 2: Color Graph
```bash
cd server
make fourcolor
```
Then run `./fourcolor [testcaseFilePath] [sequentialOrNot(true or false)]`. For example, you can run
```bash
./fourcolor testcases_fourcolor/good_40_100_4s.txt true
```
