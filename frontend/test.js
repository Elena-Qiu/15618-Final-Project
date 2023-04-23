function generate_random_lines(n, w, h) {
    local_lines = []
    for (i = 0; i < n; ++i) {
        x1 = Math.floor(Math.random(w));
        y1 = Math.floor(Math.random(h));
        x2 = Math.floor(Math.random(w));
        y2 = Math.floor(Math.random(h));
        v1 = createVector(x1, y1);
        v2 = createVector(x2, y2);
        local_lines.push([v1, v2]);
    }

    return local_lines;
}

export { generate_random_lines };