var d3Tree = (function d3Tree() {
    var trees = [];



    var Tree = function(data, w, h, p) {
        var Tree = this;
        this.margin = 10,
        this.i = 0;
        this.height = h;
        this.width = w;
        this.position = p;
        this.root = data;
        this.root.x0 = this.height / 2;
        this.root.y0 = 0;

        this.tree = d3.layout.tree()
            .size([this.height - this.margin, this.width - this.margin]);

        this.diagonal = d3.svg.diagonal()
            .projection(function(d) {
                return [d.y, d.x];
            });

        this.vis = d3.select(this.position[0]).append('svg:svg')
            .attr('width', this.width)
            .attr('height', this.height)
            .attr('style', 'position:fixed;display:inline;margin-left:' + this.position[1] + 'px; margin-top:' + this.position[2] + 'px;')
            .append('svg:g')
            .attr('transform', 'translate(' + this.margin + ',' + this.margin + ')');

        this.toggle = function(d) {
            if (d.children) {
                d._children = d.children;
                d.children = null;
            } else {
                d.children = d._children;
                d._children = null;
            }
        };

        this.update = function(source) {
            var duration = d3.event && d3.event.altKey ? 5000 : 500;

            // Compute the new tree layout.
            var nodes = this.tree.nodes(this.root).reverse();

            // Normalize for fixed-depth.
            nodes.forEach(function(d) {
                d.y = d.depth * 180;
            });

            // Update the nodes…
            var node = this.vis.selectAll("g.node")
                .data(nodes, function(d) {
                    return d.id || (d.id = ++this.i);
                });

            // Enter any new nodes at the parent's previous position.
            var nodeEnter = node.enter().append("svg:g")
                .attr("class", "node")
                .attr("transform", function(d) {
                    return "translate(" + source.y0 + "," + source.x0 + ")";
                })
                .on("click", function(d) {
                    Tree.toggle(d);
                    Tree.update(d);
                });

            nodeEnter.append("svg:circle")
                .attr("r", 1e-6)
                .style("fill", function(d) {
                    return d._children ? "lightsteelblue" : "#fff";
                });

            nodeEnter.append("svg:text")
                .attr("x", function(d) {
                    return d.children || d._children ? -10 : 10;
                })
                .attr("dy", ".35em")
                .attr("text-anchor", function(d) {
                    return d.children || d._children ? "end" : "start";
                })
                .text(function(d) {
                    return d.name;
                })
                .style("fill-opacity", 1e-6);

            // Transition nodes to their new position.
            var nodeUpdate = node.transition()
                .duration(duration)
                .attr("transform", function(d) {
                    return "translate(" + d.y + "," + d.x + ")";
                });

            nodeUpdate.select("circle")
                // .attr("r", 1.5)
                .attr("r", 4.5)
                .style("fill", function(d) {
                    return d._children ? "lightsteelblue" : "#fff";
                });

            nodeUpdate.select("text")
                .style("fill-opacity", 1);

            // Transition exiting nodes to the parent's new position.
            var nodeExit = node.exit().transition()
                .duration(duration)
                .attr("transform", function(d) {
                    return "translate(" + source.y + "," + source.x + ")";
                })
                .remove();

            nodeExit.select("circle")
                .attr("r", 1e-6);

            nodeExit.select("text")
                .style("fill-opacity", 1e-6);

            // Update the links…
            var link = this.vis.selectAll("path.link")
                .data(this.tree.links(nodes), function(d) {
                    return d.target.id;
                });

            // Enter any new links at the parent's previous position.
            link.enter().insert("svg:path", "g")
                .attr("class", "link")
                .attr("d", function(d) {
                    var o = {
                        x: source.x0,
                        y: source.y0
                    };
                    return Tree.diagonal({
                        source: o,
                        target: o
                    });
                })
                .transition()
                .duration(duration)
                .attr("d", Tree.diagonal);

            // Transition links to their new position.
            link.transition()
                .duration(duration)
                .attr("d", Tree.diagonal);

            // Transition exiting nodes to the parent's new position.
            link.exit().transition()
                .duration(duration)
                .attr("d", function(d) {
                    var o = {
                        x: source.x,
                        y: source.y
                    };
                    return Tree.diagonal({
                        source: o,
                        target: o
                    });
                })
                .remove();

            // Stash the old positions for transition.
            nodes.forEach(function(d) {
                d.x0 = d.x;
                d.y0 = d.y;
            });
        };

        this.update(this.root);
    };



    return {
        build: function(data, location) {
            var width = jQuery(location).width(),
                height = jQuery(location).height(),
                nbSquares = Math.ceil(Math.sqrt(data.length));
            width = width / nbSquares;
            height = height / nbSquares;
            data.forEach(function(el, i, data) {
                var x = (i % nbSquares) * width,
                    y = Math.floor(i / nbSquares) * height;
                trees[i] = new Tree(el, width, height, [location, x, y]);
            });

        },
    };
})();