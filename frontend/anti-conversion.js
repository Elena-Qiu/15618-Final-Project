const w = 100;
const h = 100;
// const fileName = "testcases/naive2.txt";
const fileName = "testcases/naive3.txt";

const UNDEFINED = -1;
const BOUNDARY = -2;
const FIXED = -3;
const LINE_EXPANSION = 0;
const MAX_LINE_THICKNESS = 2 * LINE_EXPANSION + 3;
const MIN_DISTANCE = MAX_LINE_THICKNESS + 2;
let nodesNum;
let origins = new Array();
let boundaries;
let edges;
let nodes_map = new Array(w * h).fill(UNDEFINED);
let completedOrigins = new Set();
let input;

const STATES = {
    INITIAL: 0,
    LINES: 1,
    SHAPES: 2,
    FINISHED: 3,
};
let state = STATES.INITIAL;

const originalBase = -100;
const originalScale = 200;
const xExpansion = w / originalScale;
const yExpansion = h / originalScale;
let black;
let white;

const EXPAND_STATES = {
    COMPLETED: 0,
    IN_PROGRESS: 1,
    FAIL: 2,
};
const DETECT_STATES = {
    OK: 0,
    STOP: 1,
    ERROR: 2,
};

function preload() {
    input = loadStrings(fileName);
}

function setup() {
    black = color("#000000");
    white = color("#FFFFFF");

    if (readTestCase() == false) {
        console.log("setup failed");
        noLoop();
    }

    pixelDensity(1);

    noSmooth();
    let canvas = createCanvas(w, h);
    canvas.id("canvas");
    canvas.parent(document.querySelector("#canvasContainer"));
    frameRate(5);
    loadPixels();
    textSize(16);
}

// iteratively expand all areas to form the final map. Follows a FSM structure
function draw() {
    if (state === STATES.INITIAL) {
        let res = drawLines();
        if (res === EXPAND_STATES.FAIL) {
            console.log("draw line failed!");
            noLoop();
            return;
        }
        console.log("draw all lines");
        state = STATES.LINES;
    } else if (state === STATES.LINES) {
        let res = expandLines();
        if (res === EXPAND_STATES.FAIL) {
            console.log("expand line failed!");
            noLoop();
            return;
        }
        console.log("expand all lines");
        state = STATES.SHAPES;
    } else if (state === STATES.SHAPES) {
        let res = expandShapes();
        if (res === EXPAND_STATES.COMPLETED) {
            console.log("expanding all finished");
            state = STATES.FINISHED;
        }
    } else if (state === STATES.FINISHED) {
        noLoop();
    } else {
        console.log("impossible!");
    }

    applyNodesMap();
    updatePixels();
}


// read the map from testcases
function readTestCase() {
    nodesNum = parseInt(input[0]);
    console.log(`reading ${nodesNum} nodes`);
    boundaries = new Array(nodesNum);
    edges = new Array(nodesNum);
    for (let i = 0; i < nodesNum; ++i) {
        boundaries[i] = [];
        edges[i] = [];
    }

    // read coordinates
    for (let i = 1; i <= nodesNum; ++i) {
        let coordinates = input[i].split(/[\s,]+/);
        let x = Math.floor(
            (parseFloat(coordinates[0]) - originalBase) * xExpansion
        );
        let y = Math.floor(
            (parseFloat(coordinates[1]) - originalBase) * yExpansion
        );
        // check collision
        for (let j = 0; j < i - 1; ++j) {
            if (dist(x, y, origins[j].x, origins[j].y) < MIN_DISTANCE) {
                console.log(
                    `node ${i - 1} and node ${j} is too close (distance: ${dist(
                        x,
                        y,
                        origins[j].x,
                        origins[j].y
                    )})`
                );
                return false;
            }
        }
        origins.push(createVector(x, y));
    }
    // read edges
    for (let i = nodesNum + 1; i < input.length; ++i) {
        if (input[i].length == 0) continue;
        let edgePair = input[i].split(/[\s,]+/);
        let idx1 = parseInt(edgePair[0]);
        let idx2 = parseInt(edgePair[1]);
        edges[idx1].push(idx2);
        edges[idx2].push(idx1);
    }
    return true;
}

// draw lines
function drawLines() {
    for (let i = 0; i < origins.length; ++i) {
        for (let j = 0; j < edges[i].length; ++j) {
            if (edges[i][j] <= i) continue;
            let va = origins[i];
            let vb = origins[edges[i][j]];
            draw_line(va.x, va.y, vb.x, vb.y, i, edges[i][j]);
        }
    }
}

/* Draw a line with certain thickness.
An own implementation because line() is always smooth on HTML5 canvas.*/
function draw_point(x, y) {
    set_nodes_map(x, y, BOUNDARY);
}

function set_nodes_map(x, y, id) {
    nodes_map[y * w + x] = id;
}

function get_nodes_map(x, y) {
    return nodes_map[y * w + x];
}

/* Draws a line with the Bresenham line algorithm.
Adapted from http://stackoverflow.com/a/4672319/811708 */
function draw_line(x0, y0, x1, y1, idx0, idx1) {
    const dx = abs(x1 - x0);
    const dy = abs(y1 - y0);
    const sx = x0 < x1 ? 1 : -1;
    const sy = y0 < y1 ? 1 : -1;
    let err = dx - dy;
    let itrx = x0;
    let itry = y0;

    while (true) {
        // add point to boundary
        let globalIdx = getGlobalIdx(itrx, itry);
        if (dist(itrx, itry, x0, y0) < dist(itrx, itry, x1, y1)) {
            if (boundaries[idx0].includes(globalIdx) == false)
                boundaries[idx0].push(globalIdx);
        } else {
            if (boundaries[idx1].includes(globalIdx) == false)
                boundaries[idx1].push(globalIdx);
        }

        // draw a point
        draw_point(itrx, itry);
        if (itrx === x1 && itry === y1) break;
        let e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            itrx += sx;
        }
        if (e2 < dx) {
            err += dx;
            itry += sy;
        }
    }
}

function getGlobalIdx(x, y) {
    if (x < 0 || x >= w) console.log(`getGlobalIdx x (${x}) is invalid!`);
    if (y < 0 || y >= h) console.log(`getGlobalIdx y (${y}) is invalid!`);
    return y * w + x;
}

// expand each line/node to a shape
function expandLines() {
    expandShapes();
}

// randomly permutate input array of any kind
function randomPermutate(inputArr) {
    let array = inputArr.slice();
    let results = [];
    let idx = 0;
    while (array.length > 0) {
        idx += Math.floor(Math.random() * 100);
        idx %= array.length;
        results.push(array.splice(idx, 1)[0]);
    }

    return results;
}

// try to expand all nodes by 1 layer
function expandShapes() {
    if (completedOrigins.length == origins.length)
        return EXPAND_STATES.COMPLETED;

    let permutated = randomPermutate([...Array(origins.length).keys()]);
    for (let i = 0; i < permutated.length; ++i) {
        let nodeIdx = permutated[i];
        if (completedOrigins.has(nodeIdx)) continue;

        if (expandNode(nodeIdx) === EXPAND_STATES.COMPLETED)
            completedOrigins.add(nodeIdx);
    }

    if (completedOrigins.size === origins.length)
        return EXPAND_STATES.COMPLETED;
    else return EXPAND_STATES.IN_PROGRESS;
}

// expand the area of the node[idx] by one layer
function expandNode(nodeIdx) {
    let newBoundary = new Array();

    let boundary = boundaries[nodeIdx];
    while (boundary.length > 0) {
        let globalIdx = boundary.pop();
        let x = globalIdx % w;
        let y = Math.floor(globalIdx / w);

        let res = detectNeighborhood(x, y, nodeIdx);
        if (res == DETECT_STATES.STOP) {
            set_nodes_map(x, y, FIXED);
            continue;
        } else if (res == DETECT_STATES.ERROR) {
            return EXPAND_STATES.FAIL;
        }

        // try to expand to 8 surrounding neighbors
        set_nodes_map(x, y, nodeIdx);
        for (let i = -1; i <= 1; ++i) {
            for (let j = -1; j <= 1; ++j) {
                let tempx = x + i;
                let tempy = y + j;
                if (i === 0 && j === 0) continue;
                if (tempx < 0 || tempx >= w || tempy < 0 || tempy >= h)
                    continue;

                let pixel = get_nodes_map(tempx, tempy);
                if (pixel === UNDEFINED) {
                    newBoundary.push(getGlobalIdx(tempx, tempy));
                    set_nodes_map(tempx, tempy, BOUNDARY);
                } else if (pixel >= 0 && pixel != nodeIdx) {
                    console.log(
                        `impossible! node ${pixel}'s inner points and node ${nodeIdx}'s inner points are adjacent!`
                    );
                    return EXPAND_STATES.FAIL;
                }
            }
        }
    }

    boundaries[nodeIdx] = newBoundary;

    if (newBoundary.length == 0) {
        return EXPAND_STATES.COMPLETED;
    }
}

// detect the neighbors of a pixel to see whether there are other friend/enermy countries
// return true if this pixel can belong to node[global_idx], false otherwise
function detectNeighborhood(x, y, nodeIdx) {
    // check whether it is the boundary of the entire map
    if (x == 0 || x == w - 1 || y == 0 || y == h - 1) return DETECT_STATES.STOP;

    // check surrounding pixels for any inner points belonging to other nodes
    for (let i = -1; i <= 1; ++i) {
        for (let j = -1; j <= 1; ++j) {
            let tempx = x + i;
            let tempy = y + j;
            if (i === 0 && j === 0) continue;
            if (tempx < 0 || tempx >= w || tempy < 0 || tempy >= h) continue;

            let pixel = get_nodes_map(tempx, tempy);
            if (pixel >= 0 && pixel != nodeIdx) return DETECT_STATES.STOP;
        }
    }

    for (let i = -MIN_DISTANCE; i <= MIN_DISTANCE; ++i) {
        for (let j = -MIN_DISTANCE; j <= MIN_DISTANCE; ++j) {
            let tempx = x + i;
            let tempy = y + j;
            if (tempx < 0 || tempx >= w || tempy < 0 || tempy >= h) continue;
            let distance = dist(tempx, tempy, x, y);
            if (distance >= MIN_DISTANCE) continue;

            // there is an inner point of a non-connected node in the neighborhood
            let pixel = get_nodes_map(tempx, tempy);
            if (
                pixel >= 0 &&
                pixel != nodeIdx &&
                edges[nodeIdx].includes(pixel) === false
            ) {
                if (distance < MAX_LINE_THICKNESS) return DETECT_STATES.ERROR;
                return DETECT_STATES.STOP;
            }
        }
    }

    return DETECT_STATES.OK;
}

// apply nodes_map to actual pixels
function applyNodesMap() {
    // TODO
    for (let y = 0; y < h; ++y) {
        for (let x = 0; x < w; ++x) {
            if (get_nodes_map(x, y) === BOUNDARY) {
                set_pixel_color(x, y, black);
            } else if (get_nodes_map(x, y) == FIXED) {
                set_pixel_color(x, y, color("#FF0000"));
            } else {
                set_pixel_color(x, y, white);
            }
        }
    }
}

// set the color of the pixel[i, j] to c
function set_pixel_color(x, y, c) {
    let d = pixelDensity();
    for (let i = 0; i < d; i++) {
        for (let j = 0; j < d; j++) {
            let index = 4 * ((y * d + j) * width * d + (x * d + i));
            pixels[index] = red(c);
            pixels[index + 1] = green(c);
            pixels[index + 2] = blue(c);
            pixels[index + 3] = alpha(c);
        }
    }
}
