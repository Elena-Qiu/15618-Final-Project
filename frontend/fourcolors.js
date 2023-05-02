/*
Some code for drawing on a canvas and then color the whole thing.
Work in progress.

Pure gold: https://p5js.org/reference
*/

// server's connection option
const IP = "http://ghc67.ghc.andrew.cmu.edu";
const PORT = "8080";
const VALID_URI = "getMapSolution";

/* Some general variables. */
let h = 20; // height of the canvas
let w = 20; // width of the canvas
let LINE_EXPANSION = 1;
let MAX_LINE_THICKNESS = 2 * LINE_EXPANSION + 3;
let EDGE_THRESHOLD = 3;

// colors
let r;
let b;
let g;
let y;
let white;
let black;

// arrays
let nodes_map; // size w*h, recording the id of the node for each pixel (-1 for undefined, -2 for pixels on edges)
let nodes_num = 0;
let edges; // adjacent list: edges[i] is an array with the indices j such that node i and node j are connected
let edges_num = 0;
let colors = [];

const STATES = {
    DRAWING: 0,
    IMAGE_LOADED: 1,
    START_SOLVING: 2,
    SOLVING: 3,
    FINISHED: 4,
};

let start, end;
let dragging = false; // boolean, whether or not the mouse is dragging
let state = STATES.DRAWING; // stop updating the patterns
let solve_start; // time the solving process starts
let canvas;

/* Setting up canvas. */
function setup() {
    r = color('#EAD0D1');
    b = color('#C1CBD7');
    g = color('#B5C4B1');
    y = color('#FAEAD3');
    white = color("#FFFFFF");
    black = color("#000000");
    colors = [r, b, g, y];

    pixelDensity(1);

    update_status("Starting up.");
    noSmooth();
    let canvas = createCanvas(w, h);
    canvas.id("canvas");
    canvas.parent(document.querySelector("#canvasContainer"));
    loadPixels();
    frameRate(30);
    textSize(16);
    // start = createVector(w / 2, h / 2);

    nodes_map = new Array(w * h).fill(-1);
    for (let y = 0; y < h; ++y) {
        if (y === 0 || y === h - 1) {
            for (let x = 0; x < w; ++x) set_nodes_map(x, y, -2);
        } else {
            set_nodes_map(0, y, -2);
            set_nodes_map(w - 1, y, -2);
        }
    }
}

/* Main loop */
function draw() {
    switch (state) {
        case STATES.DRAWING:
            displayNodesMap();
            break;
        case STATES.IMAGE_LOADED:
            break;
        case STATES.START_SOLVING:
            state = STATES.SOLVING;
            solve(() => {
                state = STATES.FINISHED;
                let time_spent_total = millis() - solve_start;
                if (time_spent_total < 1000)
                    update_status(`spent ${time_spent_total} ms for solving`);
                else
                    update_status(`spent ${time_spent_total / 1000} s for solving`);
            });
            break;
        default:
            break;
    }
}

function displayNodesMap() {
    for (let i = 0; i < w; ++i) {
        for (let j = 0; j < h; ++j) {
            if (get_nodes_map(i, j) === -2)
                set_pixel_color(i, j, black);
        }
    }
    updatePixels();

    if (dragging) draw_line(start.x, start.y, end.x, end.y);
}

/* Draw a line with certain thickness.
An own implementation because line() is always smooth on HTML5 canvas.*/
function draw_point(x, y) {
    for (let i = 0; i < 2 * LINE_EXPANSION + 1; i++) {
        for (let j = 0; j < 2 * LINE_EXPANSION + 1; j++) {
            let px = x - LINE_EXPANSION + i;
            let py = y - LINE_EXPANSION + j;
            if (px >= 0 && py >= 0 && px < w && py < h) point(px, py);
        }
    }
}

/* Draws a line with the Bresenham line algorithm.
Adapted from http://stackoverflow.com/a/4672319/811708 */
function draw_line(x0, y0, x1, y1) {
    let dx = abs(x1 - x0);
    let dy = abs(y1 - y0);
    let sx = x0 < x1 ? 1 : -1;
    let sy = y0 < y1 ? 1 : -1;
    let err = dx - dy;

    while (true) {
        draw_point(x0, y0);
        // console.log(`draw point ${x0}, ${y0}`);
        if (x0 === x1 && y0 === y1) break;
        let e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

function set_nodes_map_with_line(x0, y0, x1, y1) {
    let dx = abs(x1 - x0);
    let dy = abs(y1 - y0);
    let sx = x0 < x1 ? 1 : -1;
    let sy = y0 < y1 ? 1 : -1;
    let err = dx - dy;

    while (true) {
        for (let i = 0; i < 2 * LINE_EXPANSION + 1; i++) {
            for (let j = 0; j < 2 * LINE_EXPANSION + 1; j++) {
                let px = x0 - LINE_EXPANSION + i;
                let py = y0 - LINE_EXPANSION + j;
                if (px >= 0 && py >= 0 && px < w && py < h) {
                    set_nodes_map(px, py, -2);
                    console.log(`set nodes map ${px}, ${py}`);
                }
            }
        }
        if (x0 === x1 && y0 === y1) break;
        let e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

/* mouse input */
function mouseDragged() {
    end = createVector(Math.floor(mouseX), Math.floor(mouseY));
    dragging = true;
}

function mousePressed() {
    start = createVector(Math.floor(mouseX), Math.floor(mouseY));
}

function mouseReleased() {
    dragging = false;
    end = createVector(Math.floor(mouseX), Math.floor(mouseY));
    // start = createVector(end.x, end.y); // build map by single clicks

    set_nodes_map_with_line(start.x, start.y, end.x, end.y);
}

function set_nodes_map(x, y, id) {
    nodes_map[y * w + x] = id;
}

function get_nodes_map(x, y) {
    return nodes_map[y * w + x];
}

function print_nodes_map() {
    for (let y = 0; y < h; ++y) {
        for (let x = 0; x < w; ++x) {
            console.log(get_nodes_map(x, y));
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

/* Main solving method.
The solving takes place in stages and logs its progress. */
async function solve(callback) {
    update_status("solving at backend");
    loadPixels();
    await solve_graph();
    updatePixels();
    update_status("solved");
    callback();
}

async function solve_graph() {
    async function solveAtServer(input) {
        const response = await fetch(IP + ":" + PORT + "/" + VALID_URI, {
            method: "POST",
            mode: "cors",
            body: input,
        });
        const responseText = await response.text();
        return responseText;
    }

    let input = convertNodeMapToString(pixels);
    console.log(input);
    let solutionStr = await solveAtServer(input);
    convertStringToNodeMap(solutionStr);
    updatePixelsWithNodeMap();
}

function updatePixelsWithNodeMap() {
    for (let y = 0; y < h; y++) {
        for (let x = 0; x < w; x++) {
            let id = get_nodes_map(x, y);
            if (id >= 0 && id <= 3) {
                set_pixel_color(x, y, colors[id]);
            } 
            // else if (id == -2) {
            //     set_pixel_color(x, y, black);
            // }
        }
    }
}

function convertNodeMapToString() {
    // let string = new TextDecoder().decode(pixels);
    // let string = String.fromCharCode.apply(null, pixels);
    let string = "" + w + "\n" + h + "\n" + nodes_map.join(",");
    return string + ",";
}

function convertStringToNodeMap(solutionStr) {
    nodes_map = Array.from(solutionStr.split(",").map((c) => parseInt(c)));
}

function update_status(s) {
    document.getElementById("log").innerHTML += "<br>" + s;
    console.log(s);
}

/* -------- buttons -------- */

function button_solve() {
    if (state === STATES.DRAWING || state === STATES.IMAGE_LOADED) {
        solve_start = millis();
        state = STATES.START_SOLVING;
    }
    frameRate(5);
}

function button_reset() {
    frameRate(30);
    nodes_map = new Array(w * h).fill(-1);
    for (let y = 0; y < h; ++y) {
        if (y === 0 || y === h - 1) {
            for (let x = 0; x < w; ++x) set_nodes_map(x, y, -2);
        } else {
            set_nodes_map(0, y, -2);
            set_nodes_map(w - 1, y, -2);
        }
    }
    state = STATES.DRAWING;
    document.getElementById("log").innerHTML = "";
    nodes = [];
    edges = [];
    nodes_num = 0;
    edges_num = 0;
}

function button_generate_image() {
    let img_data = document.getElementsByTagName("canvas")[0].toDataURL();
    if (document.getElementById("data").innerHTML === "") {
        document.getElementById("data").innerHTML =
            'Snapshots: <br> <img height="50" src="' + img_data + '">';
    } else {
        document.getElementById("data").innerHTML +=
            '<img height="50" src="' + img_data + '">';
    }
}

function button_load_image(s) {
    button_reset();
    let canvas = document.getElementById("canvas");
    let context = canvas.getContext("2d");
    let img = new Image();
    state = STATES.IMAGE_LOADED;
    img.onload = function () {
        context.drawImage(this, 0, 0, w, h);
        loadPixels();
        set_nodes_map_with_pixels();
    };
    img.src = s;
}


function set_nodes_map_with_pixels() {
    for (let j = 0; j < h; ++j) {
        for (let i = 0; i < w; ++i) {
            if (check_pixel_color(i, j, white)) {
                set_nodes_map(i, j, -1);
            } else {
                set_nodes_map(i, j, -2);
            }
        }
    }

    for (let y = 0; y < h; ++y) {
        if (y === 0 || y === h - 1) {
            for (let x = 0; x < w; ++x) set_nodes_map(x, y, -2);
        } else {
            set_nodes_map(0, y, -2);
            set_nodes_map(w - 1, y, -2);
        }
    }
}


// check whether the color on pixel[x, y] is c
function check_pixel_color(x, y, c) {
    let pc = get_pixel_color(x, y);
    return compare_colors(pc, c);
}


// get the color of the pixel[i, j]
function get_pixel_color(x, y) {
    let d = pixelDensity();
    let base = 4 * (y * d * width * d + x * d);
    return color(
        pixels[base],
        pixels[base + 1],
        pixels[base + 2],
        pixels[base + 3]
    );
}


// compare whether two colors are equal
function compare_colors(c1, c2) {
    if (
        red(c1) === red(c2) &&
        green(c1) === green(c2) &&
        blue(c1) === blue(c2) &&
        alpha(c1) === alpha(c2)
    )
        return true;
    return false;
}