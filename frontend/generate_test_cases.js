/*
Some code for drawing on a canvas and then color the whole thing.
Work in progress.

Pure gold: https://p5js.org/reference
*/

NUM_LINES = 100;
ELLIPSE_PERCENTAGE = 0.4;

ELLIPSE_NUM = Math.floor(NUM_LINES * ELLIPSE_PERCENTAGE);
LINE_NUM = NUM_LINES - ELLIPSE_NUM;

// server's connection option
const IP = "http://ghc67.ghc.andrew.cmu.edu";
const PORT = "8080";
const VALID_URI = "getMapSolution";

/* Some general variables. */
let h = 1500;        // height of the canvas
let w = 1500;        // width of the canvas
let LINE_EXPANSION = 0;
let MAX_LINE_THICKNESS = 2 * LINE_EXPANSION + 3;
let EDGE_THRESHOLD = 3;

// colors
let black;
let white;
let greencolor;
let pink;
let r;
let b;
let g;
let y;

// arrays
let colors = [r, b, g, y];
let nodes_map; // size w*h, recording the id of the node for each pixel (-1 for undefined, -2 for pixels on edges)
let nodes_num = 0;
let edges; // adjacent list: edges[i] is an array with the indices j such that node i and node j are connected
let edges_num = 0;
let marginal_points = [];   // array of marginal nodes (of type p5.vector)
let visible_edges = []; // array of lines that cross an edge (each line is an array of two vectors)
let node_mapping = [];  // node_mapping[i] is the color for node i
let color_map = [];
let lines = [];

let stop_updating = false;  // stop updating the patterns
let image_loaded = false;   // whether or not the image is loaded
let solve_start;        // time the solving process starts
let solve_stage = -2;       // the stage of solving process

function generate_random_lines() {
    local_lines = []
    local_ellipse = []
    for (let i = 0; i < LINE_NUM; ++i) {
        let x1 = Math.floor(Math.random() * w);
        let y1 = Math.floor(Math.random() * h);
        let x2 = Math.floor(Math.random() * w);
        let y2 = Math.floor(Math.random() * h);
        let v1 = createVector(x1, y1);
        let v2 = createVector(x2, y2);
        local_lines.push([v1, v2]);
    }
    for (let i = 0; i < ELLIPSE_NUM; ++i) {
        let x = Math.floor(Math.random() * w);
        let y = Math.floor(Math.random() * h);
        let wi = Math.floor(Math.random() * w);
        let he = Math.floor(Math.random() * h);
        let v1 = createVector(x, y);
        let v2 = createVector(wi, he);
        local_ellipse.push([v1, v2]);
    }

    return (local_lines, local_ellipse);
}

/* Setting up canvas. */
function setup() {
    black = color('#000000');
    white = color('#FFFFFF');
    greencolor = color('#00FF00');
    pink = color('#FF00FF');

    r = color('#E2041B');
    b = color('#048AD0');
    g = color('#338823');
    y = color('#FAEA04');

    update_status("Starting up.");
    noSmooth();
    pixelDensity(1);
    let canvas = createCanvas(w, h);
    canvas.id("canvas");
    canvas.parent(document.querySelector("#canvasContainer"));
    loadPixels();
    frameRate(30);
    textSize(16);
    lines, ellipse = generate_random_lines();

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
    solve();
    
    if (stop_updating) {
        update_pixels();
        frameRate(5);
    }
    else
        display_lines();
}

function display_lines() {
        background(255);
        stroke(0);
        fill(255);
        rect(0, 0, w-1, h-1);

        // draw lines
    for (let i = 0; i < lines.length; i++) {
        let a = lines[i][0];
        let b = lines[i][1];
            draw_line(a.x, a.y, b.x, b.y);
        }
}

/* Draw a line with certain thickness.
An own implementation because line() is always smooth on HTML5 canvas.*/
function draw_point(x, y) {
    for (let i = 0; i < 2 * LINE_EXPANSION + 1; i++) {
        for (let j = 0; j < 2 * LINE_EXPANSION + 1; j++) {
            let px = x - LINE_EXPANSION + i;
            let py = y - LINE_EXPANSION + j;
            if (px < w && py < h)
                point(px, py);
        }
    }
}

/* Draws a line with the Bresenham line algorithm.
Adapted from http://stackoverflow.com/a/4672319/811708 */
function draw_line(x0, y0, x1, y1) {
    let dx = abs(x1 - x0);
    let dy = abs(y1 - y0);
    let sx = (x0 < x1) ? 1 : -1;
    let sy = (y0 < y1) ? 1 : -1;
    let err = dx - dy;

    while (true) {
        draw_point(x0, y0);
        if ((x0 === x1) && (y0 === y1)) break;
        let e2 = 2*err;
        if (e2 > -dy)   { err -= dy; x0  += sx; }
        if (e2 < dx)    { err += dx; y0  += sy; }
   }
}

function set_nodes_map_with_line(x0, y0, x1, y1) {
    let dx = abs(x1 - x0);
    let dy = abs(y1 - y0);
    let sx = (x0 < x1) ? 1 : -1;
    let sy = (y0 < y1) ? 1 : -1;
    let err = dx - dy;

    while (true) {
        for (let i = 0; i < 2 * LINE_EXPANSION + 1; i++) {
            for (let j = 0; j < 2 * LINE_EXPANSION + 1; j++) {
                let px = x0 - LINE_EXPANSION + i;
                let py = y0 - LINE_EXPANSION + j;
                if (px < w && py < h) {
                    set_nodes_map(px, py, -2);
                }
            }
        }
        if ((x0 === x1) && (y0 === y1)) break;
        let e2 = 2*err;
        if (e2 > -dy)   { err -= dy; x0  += sx; }
        if (e2 < dx)    { err += dx; y0  += sy; }
   }
}

// get the color of the pixel[i, j]
function get_pixel_color(x, y) {
    let d = pixelDensity();
    let base = 4 * (y * d * width * d + x * d);
    return color(pixels[base], pixels[base + 1], pixels[base + 2], pixels[base + 3]);
}

// set the color of the pixel[i, j] to c
function set_pixel_color(x, y, c) {
    let d = pixelDensity();
    for (let i = 0; i < d; i++) {
        for (let j = 0; j < d; j++) {
            let index = 4 * ((y * d + j) * width * d + (x * d + i));
            pixels[index] = red(c);
            pixels[index+1] = green(c);
            pixels[index+2] = blue(c);
            pixels[index+3] = alpha(c);
        }
    }
}

// compare whether two colors are equal
function compare_colors(c1, c2) {
    if (red(c1) === red(c2) &&
        green(c1) === green(c2) &&
        blue(c1) === blue(c2) &&
        alpha(c1) === alpha(c2))
        return true;
    return false;
}

// check whether the color of a certain pixel is of color c
function check_pixel_color(x, y, c) {
    let pc = get_pixel_color(x, y);
    return compare_colors(pc, c);
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

/* Main solving method.
The solving takes place in stages and logs its progress. */
function solve() {
    if (solve_stage === -2) {
        for (let i = 0; i < NUM_LINES; i++) {
            let line = lines[i];
            set_nodes_map_with_line(line[0].x, line[0].y, line[1].x, line[1].y);
        }
        solve_stage ++;
    } else if (solve_stage === -1)
        return;
    else if (solve_stage === 0) {
        update_status("Loading pixels...");
        loadPixels();

        image_loaded = false;
        solve_stage++;
    }
    else if (solve_stage === 1 && check_time()) { solve_stage++; }
    else if (solve_stage === 2) {
        update_status("Analyzing areas & finding nodes...");
        find_nodes();
        
        update_pixels();
        update_status("Found a total of " + nodes_num + " areas/nodes.");
        edges = new Array(nodes_num);
        for (let i = 0; i < edges.length; ++i)
            edges[i] = [];
        solve_stage++;
    }
    else if (solve_stage === 3 && check_time()) { solve_stage++; }
    else if (solve_stage === 4) {
        update_status("Analyzing marginal points & finding edges...");
        find_edges();
        update_pixels();
        update_status("Found a total of " + edges_num + " edges.");
        solve_stage++;
    }
    else if (solve_stage === 5 && check_time()) { solve_stage++; }
    else if (solve_stage === 6) {
        update_status("Building & solving graph, stand by...");
        solve_graph();
        update_status("Finished.");
        update_pixels();
        solve_stage++;
    }
}

function solve_graph() {
    async function solveAtServer(input) {
        const response = await fetch(
            IP + ":" + PORT + "/" + VALID_URI, 
            {
                method: 'POST',
                mode: "cors",
                body: input
            }
        );
        const responseText = await response.text();
        return responseText
    }

    let input = nodes_num.toString() + "\n";
    for (let i = 0; i < nodes_num; ++i) {
        let local_edges = edges[i];
        for (let j = 0; j < local_edges.length; ++j) {
            input += i + " " + edges[i][j] + "\n";
        }
    }
    
    let writer = createWriter("test.txt");
    writer.print(input);
    writer.close();
    writer.clear();

    // decode the output

}

function check_time() {
    return (solve_start + solve_stage * 500) < millis();
}

function update_pixels() {
    if (image_loaded == false)
        updatePixels();

    // update the color of pixels according to the mapping
    

    if (node_mapping.length > 0) {
        for (let y = 0; y < h; ++y) {
            for (let x = 0; x < w; ++x) {
                let idx = get_nodes_map(x, y);
                let c = node_mapping[idx];
                set_pixel_color(x, y, c);
                }
            }
        node_mapping = [];
    }
    else {
        // color marginal points to pink
        stroke(pink);
        strokeWeight(2);
        for (let i = 0; i < marginal_points.length; i++) {
            mp = marginal_points[i];
            for (let j = 0; j < mp.length; j++) {
                let p = mp[j];
                point(p.x, p.y);
            }
        }

        // indicate visible edges using green color
        stroke(greencolor);
        strokeWeight(4);
        for (let i = 0; i < visible_edges.length; i++) {
            let pts = visible_edges[i];
            line(pts[0].x, pts[0].y, pts[1].x, pts[1].y);
        }
    }

    stroke(black);
    strokeWeight(1);
}

function update_status(s) {
    document.getElementById("log").innerHTML += '<br>' + s;
    console.log(s);
}

/* Loop through pixels.
If a white one was found, trigger BFS */
function find_nodes() {
    for (let y = 0; y < h; y++) {
        for (let x = 0; x < w; x++) {
            if (get_nodes_map(x, y) === -1) {
                let local_marginal_points = fill_area(x, y, nodes_num);
                if (local_marginal_points.length > 0) {
                    nodes_num ++;
                    marginal_points.push(local_marginal_points);
                }
            }
        }
    }
}

/* Rand-pixel pro node identifizieren, um diese mit anderen nodes zu vergleichen*/
function find_edges() {
    let temp_edges = new Array(nodes_num);
    for (let i = 0; i < nodes_num; ++i)
        temp_edges[i] = [[], []];

    // compare and check if nodes have an edge
    for (let i = 0; i < nodes_num; i++) {
        let visited_neighbors = new Set();  // neighbors that have been visited
        let mp = marginal_points[i];
        for (let j = 0; j < mp.length; ++j) {
            // check the surrounding points of mp[j] to see if they belong to other nodes
            let p = mp[j];
            for (let k = Math.floor(-MAX_LINE_THICKNESS); k < Math.ceil(MAX_LINE_THICKNESS); ++k) {
                for (let l = Math.floor(-MAX_LINE_THICKNESS); l < Math.ceil(MAX_LINE_THICKNESS); ++l) {
                    let tempx = p.x + k;
                    let tempy = p.y + l;
                    // if out of distance, skip
                    if (dist(tempx, tempy, p.x, p.y) >= MAX_LINE_THICKNESS)
                        continue;
                    let global_idx = tempy * w + tempx;
                    // if already visited, skip
                    if (visited_neighbors.has(global_idx))
                        continue;
                    visited_neighbors.add(global_idx);

                    let tempid = get_nodes_map(tempx, tempy);
                    if (tempid >= 0 && tempid != i) {
                        // if already added as an edge, skip
                        if (edges[i].includes(tempid))
                            continue;
                        // if already added as a temporary edge, increment the counter
                        let idx = temp_edges[i][0].indexOf(tempid);
                        if (idx != -1) {
                            temp_edges[i][1][idx] += 1;
                            if (temp_edges[i][1][idx] > EDGE_THRESHOLD) {
                        visible_edges.push([createVector(tempx, tempy), p]);
                        edges[i].push(tempid);
                        edges[tempid].push(i);
                        edges_num ++;
}
            }
                        // add the edge to temp edges array
                        temp_edges[i][0].push(tempid);
                        temp_edges[i][1].push(1);
                        temp_edges[tempid][0].push(i);
                        temp_edges[tempid][1].push(1);
                    }
                }
        }
    }
}
}

/* Fill area with color c using BFS. Find marginal points at the same time */
function fill_area(x, y, id) {
    let n = 0;
    let queue = new Array();
    queue.push(createVector(x, y));
    let local_marginal_points = [];

    while (queue.length > 0) {
        // pop point from list and color it
        let p = queue.pop();
        let is_marginal = false;
        set_nodes_map(p.x, p.y, id);
        n += 1;

        // check neighbors
        if (get_nodes_map(p.x, p.y + 1) === -1)   queue.push(createVector(p.x, p.y + 1));
        else if (get_nodes_map(p.x, p.y + 1) === -2)    is_marginal = true;
        if (get_nodes_map(p.x, p.y - 1) === -1)   queue.push(createVector(p.x, p.y - 1));
        else if (get_nodes_map(p.x, p.y - 1) === -2)    is_marginal = true;
        if (get_nodes_map(p.x + 1, p.y) === -1)   queue.push(createVector(p.x + 1, p.y));
        else if (get_nodes_map(p.x + 1, p.y) === -2)    is_marginal = true;
        if (get_nodes_map(p.x - 1, p.y) === -1)   queue.push(createVector(p.x - 1, p.y));
        else if (get_nodes_map(p.x - 1, p.y) === -2)    is_marginal = true;

        if (is_marginal)
            local_marginal_points.push(p);
    }
    // one-pixel bug: if only one pixel, don't count it as separate area
    if (n === 1) {
        set_nodes_map(x, y, -2);
        return [];
    }
    else
        return local_marginal_points;
}


/* -------- buttons -------- */

function button_solve() {
    solve_start = millis();
    solve_stage = 0;
    stop_updating = true;
}

function button_reset() {
    frameRate(30);
    lines = new Array();
    stop_updating = false;
    solve_stage = -2;
    image_loaded = false;
    document.getElementById("log").innerHTML = '';
    nodes = [];
    edges = [];
    nodes_num = 0;
    edges_num = 0;
    visible_edges = [];
    marginal_points = [];
    node_mapping = [];
}

function button_generate_image() {
    let img_data = document.getElementsByTagName("canvas")[0].toDataURL();
    if (document.getElementById("data").innerHTML === '') {
        document.getElementById("data").innerHTML = 'Snapshots: <br> <img height="50" src="' + img_data + '">'
    }
    else {
        document.getElementById("data").innerHTML += '<img height="50" src="' + img_data + '">'
    }
}

function button_load_image(s) {
    button_reset();
    let context = document.getElementById("canvas").getContext('2d');
    let img = new Image();
    stop_updating = true;
    image_loaded = true;
    img.onload = function() {
        context.drawImage(this, 0, 0, canvas.width, canvas.height);
    }
    img.src = s;
}