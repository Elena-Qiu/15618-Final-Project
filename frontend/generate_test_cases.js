/*
Some code for drawing on a canvas and then color the whole thing.
Work in progress.

Pure gold: https://p5js.org/reference
*/

SHAPE_CATEGORY_NUM = 4; // arc, ellipse, quad, triangle

SHAPE_NUM = 700;
MAX_SCALE = 400;
const SHAPES = {
    ARC: 0,
    ELLIPSE: 1,
    QUAD: 2,
    TRIANGLE: 3
};

// server's connection option
const IP = "http://ghc67.ghc.andrew.cmu.edu";
const PORT = "8080";
const VALID_URI = "getMapSolution";

/* Some general variables. */
let h = 1000; // height of the canvas
let w = 1000; // width of the canvas
MIN_SCALE = Math.min(h, w) / 10;
let LINE_EXPANSION = 0;
let MAX_LINE_THICKNESS = 2 * LINE_EXPANSION + 3;
let EDGE_THRESHOLD = 3;

// colors
let black;
let white;

// arrays
let nodes_map; // size w*h, recording the id of the node for each pixel (-1 for undefined, -2 for pixels on edges)
let nodes_num = 0;
let edges; // adjacent list: edges[i] is an array with the indices j such that node i and node j are connected
let edges_num = 0;
let marginal_points = []; // array of marginal nodes (of type p5.vector)
let visible_edges = []; // array of lines that cross an edge (each line is an array of two vectors)
let lines = [];

const STATES = {
    DRAWING: 0,
    DISPLAYED: 1,
    SOLVING: 2,
    FINISHED: 3
};
let state = STATES.DRAWING;  // stop updating the patterns

/* Setting up canvas. */
function setup() {
    black = color("#000000");
    white = color("#FFFFFF");

    pixelDensity(1);

    update_status("Starting up.");
    noSmooth();
    let canvas = createCanvas(w, h);
    canvas.id("canvas");
    canvas.parent(document.querySelector("#canvasContainer"));
    loadPixels();
    frameRate(30);
    textSize(16);

    nodes_map = new Array(w * h).fill(-1);

    // noFill();
}

/* Main loop */
function draw() {
    switch (state) {
        case STATES.DRAWING:
            state = STATES.DISPLAYED;
            display_shapes();
            break;
        case STATES.SOLVING:
            solve();
            break;
        default:
            break;
    }
}

function display_shapes() {
    background(255);
    stroke(0);
    fill(255);

    // draw shapes
    for (let i = 0; i < SHAPE_NUM; ++i) {
        let category = random_int(SHAPE_CATEGORY_NUM);
        // let category = SHAPES.ARC;
        switch (category) {
        case SHAPES.ARC:
            draw_arc();
            break;
        case SHAPES.ELLIPSE:
            draw_ellipse();
            break;
        case SHAPES.QUAD:
            draw_quad();
            break;
        case SHAPES.TRIANGLE:
            draw_triangle();
            break;
        default:
            console.log("impossible");
        }
    }

    noFill();
    rect(0, 0, w - 1, h - 1);
}

function random_int(i) {
    return Math.floor(Math.random() * i);
}

function random(min, max) {
    return min + Math.random() * max;
}

function random_int(min, max) {
    return Math.floor(random(min, max));
}

function draw_arc() {
    let cx = random_int(w);
    let cy = random_int(h);
    let max_d = MAX_SCALE / 2;
    let min_d = MIN_SCALE / 3;
    let dx = random_int(min_d, max_d);
    let dy = random_int(min_d, max_d);
    let as = Math.random();
    let ae = as + Math.random() * TWO_PI;
    arc(cx, cy, dx, dy, as, ae, PIE);
}

function draw_ellipse() {
    let cx = random_int(w);
    let cy = random_int(h);
    let max_d = MAX_SCALE / 3;
    let min_d = MIN_SCALE / 3;
    let dx = random_int(min_d, max_d);
    let dy = random_int(min_d, max_d);
    ellipse(cx, cy, dx, dy);
}

function draw_quad() {
    let cx = random_int(w);
    let cy = random_int(h);
    let max_d = MAX_SCALE;
    let min_d = MIN_SCALE;
    quad(
        cx + random_int(min_d, max_d),
        cy + random_int(min_d, max_d),
        cx + random_int(min_d, max_d),
        cy + random_int(min_d, max_d),
        cx + random_int(min_d, max_d),
        cy + random_int(min_d, max_d),
        cx + random_int(min_d, max_d),
        cy + random_int(min_d, max_d)
    );
}

function draw_triangle() {
    let cx = random_int(w);
    let cy = random_int(h);
    let max_d = MAX_SCALE;
    let min_d = MIN_SCALE;
    triangle(
        cx + random_int(min_d, max_d),
        cy + random_int(min_d, max_d),
        cx + random_int(min_d, max_d),
        cy + random_int(min_d, max_d),
        cx + random_int(min_d, max_d),
        cy + random_int(min_d, max_d)
    );
}

function set_nodes_map_with_shapes() {
    for (let j = 0; j < h; ++j) {
        for (let i = 0; i < w; ++i) {
            if (check_pixel_is_white(i, j)) {
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

function check_pixel_is_white(x, y) {
    let c = get_pixel_color(x, y);
    if (red(c) > 192 && green(c) > 192 && blue(c) > 192 && alpha(c) > 192)
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

function print_pixels() {
    for (let y = 0; y < h; ++y) {
        for (let x = 0; x < w; ++x) {
            let c = get_pixel_color(x, y);
            console.log(`${red(c)}, ${green(c)}, ${blue(c)}, ${alpha(c)}`);
        }
    }
}

/* Main solving method.
The solving takes place in stages and logs its progress. */
function solve() {
    if ((state = STATES.DISPLAYED)) {
        update_status("Loading pixels...");
        loadPixels();

        update_status("updating nodes_map...");
        set_nodes_map_with_shapes();
        nodes_map_copy = new Array(nodes_map.length);
        // nodes_map_copy = read_from_file("100x100_76_84_inputs.txt");
        for (let i = 0; i < nodes_map.length; ++i)
            nodes_map_copy[i] = nodes_map[i];
        // print_pixels();
        // print_nodes_map();

        update_status("Analyzing areas & finding nodes...");
        find_nodes();

        update_status("Found a total of " + nodes_num + " areas/nodes.");
        edges = new Array(nodes_num);
        for (let i = 0; i < edges.length; ++i) edges[i] = [];

        update_status("Analyzing marginal points & finding edges...");
        find_edges();
        update_status("Found a total of " + edges_num + " edges.");

        update_status("generating testcase...");
        generate_testcase_nodes_map(nodes_map_copy);
        generate_testcase_graph();
        update_status("Finished.");

        state = STATES.FINISHED;
    }
}

function read_from_file(fileName) {
    // TODO
}

function generate_testcase_nodes_map(nodes_map_copy) {
    let input = w + "\n" + h + "\n";
    for (let i = 0; i < nodes_map_copy.length; ++i) {
        input += nodes_map_copy[i] + "\n";
    }

    let writer = createWriter(`${w}x${h}_${nodes_num}_${edges_num}_inputs.txt`);
    writer.print(input);
    writer.close();
    writer.clear();
}

function generate_testcase_graph() {
    let input = nodes_num + "\n";
    for (let i = 0; i < nodes_num; ++i) {
        input += "\n";
    }
    for (let i = 0; i < nodes_num; ++i) {
        let neighbors = new Array();
        for (let j = 0; j < edges[i].length; ++j) {
            if (edges[i][j] <= i)
                continue;
            neighbors.push(edges[i][j]);
        }
        neighbors.sort(function(a, b){return a - b;});
        for (let j = 0; j < neighbors.length; ++j)
            input += i + " " + neighbors[j] + "\n";
    }

    let writer = createWriter(`${w}x${h}_${nodes_num}_${edges_num}_answer.txt`);
    writer.print(input);
    writer.close();
    writer.clear();
}

function update_status(s) {
    document.getElementById("log").innerHTML += "<br>" + s;
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
                    nodes_num++;
                    marginal_points.push(local_marginal_points);
                }
            }
        }
    }
}

/* Rand-pixel pro node identifizieren, um diese mit anderen nodes zu vergleichen*/
function find_edges() {
    let temp_edges = new Array(nodes_num);
    for (let i = 0; i < nodes_num; ++i) temp_edges[i] = [[], []];

    // compare and check if nodes have an edge
    for (let i = 0; i < nodes_num; i++) {
        let visited_neighbors = new Set(); // neighbors that have been visited
        let mp = marginal_points[i];
        for (let j = 0; j < mp.length; ++j) {
            // check the surrounding points of mp[j] to see if they belong to other nodes
            let p = mp[j];
            for (
                let k = Math.floor(-MAX_LINE_THICKNESS);
                k < Math.ceil(MAX_LINE_THICKNESS);
                ++k
            ) {
                for (
                    let l = Math.floor(-MAX_LINE_THICKNESS);
                    l < Math.ceil(MAX_LINE_THICKNESS);
                    ++l
                ) {
                    let tempx = p.x + k;
                    let tempy = p.y + l;
                    // if out of distance, skip
                    if (tempx < 0 || tempx >= w || tempy < 0 || tempy >= h)  continue;
                    if (dist(tempx, tempy, p.x, p.y) >= MAX_LINE_THICKNESS)
                        continue;
                    let global_idx = tempy * w + tempx;
                    // if already visited, skip
                    if (visited_neighbors.has(global_idx)) continue;
                    visited_neighbors.add(global_idx);

                    let tempid = get_nodes_map(tempx, tempy);
                    if (tempid >= 0 && tempid != i) {
                        // if already added as an edge, skip
                        if (edges[i].includes(tempid)) continue;
                        // if already added as a temporary edge, increment the counter
                        let idx = temp_edges[i][0].indexOf(tempid);
                        if (idx != -1) {
                            temp_edges[i][1][idx] += 1;
                            if (temp_edges[i][1][idx] > EDGE_THRESHOLD) {
                                visible_edges.push([
                                    createVector(tempx, tempy),
                                    p,
                                ]);
                                edges[i].push(tempid);
                                edges[tempid].push(i);
                                edges_num++;
                            }
                        } else {
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
}

/* Fill area with color c using BFS. Find marginal points at the same time */
function fill_area(x, y, id) {
    let n = 0;
    let queue = new Array();
    let visited = new Set();
    queue.push(createVector(x, y));
    let local_marginal_points = [];

    function should_visit(x, y) {
        if (visited.has(y * w + x))
            return false;
        
        visited.add(y * w + x);
        if (get_nodes_map(x, y) === -1)
            return true;
    }

    while (queue.length > 0) {
        // pop point from list and color it
        let p = queue.pop();
        let is_marginal = false;
        set_nodes_map(p.x, p.y, id);
        n += 1;

        // check neighbors
        if (should_visit(p.x, p.y + 1))
            queue.push(createVector(p.x, p.y + 1));
        else if (get_nodes_map(p.x, p.y + 1) === -2) is_marginal = true;
        if (should_visit(p.x, p.y - 1))
            queue.push(createVector(p.x, p.y - 1));
        else if (get_nodes_map(p.x, p.y - 1) === -2) is_marginal = true;
        if (should_visit(p.x + 1, p.y))
            queue.push(createVector(p.x + 1, p.y));
        else if (get_nodes_map(p.x + 1, p.y) === -2) is_marginal = true;
        if (should_visit(p.x - 1, p.y))
            queue.push(createVector(p.x - 1, p.y));
        else if (get_nodes_map(p.x - 1, p.y) === -2) is_marginal = true;

        if (is_marginal) local_marginal_points.push(p);
    }
    // one-pixel bug: if only one pixel, don't count it as separate area
    if (n === 1) {
        set_nodes_map(x, y, -2);
        return [];
    } else return local_marginal_points;
}

/* -------- buttons -------- */

function button_solve() {
    solve();
}

function button_reset() {
    frameRate(30);
    lines = new Array();
    state = STATES.DRAWING;
    document.getElementById("log").innerHTML = "";
    nodes = [];
    edges = [];
    nodes_num = 0;
    edges_num = 0;
    visible_edges = [];
    marginal_points = [];
}
