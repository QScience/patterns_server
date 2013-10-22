/*
 * When doing d3Tree.build(data, location):
 * - location: should be a jQuery-like selector string. (Ex: '#mydiv') The element should have a width and height.
 * - data: an array of objects. Here is a minimum data example:
 * [
 *  {name: "myName", children: [
 *      ...The children of this node... (objects with children and name keys)
 *  ] },
 * ]
 */

var d3Tree = (function() {
    var trees = [],
        minWidth = 200,
        minHeight = 40,
        minLeafRadius = 2.0,
        maxLeafRadius = 7.0,
        minBranchLength = 40,
        marginSVG = 10,
        svgDiv,
        bubbleDiv,
        getRandomColor,
        bubbleTimeOut,
        bubbleShowDuration = 3500,
        zoomFactor = 0.25;


    getRandomColor = function() {
        var letters = '0123456789ABCDEF'.split('');
        var color = '#';
        for (var i = 0; i < 6; i++) {
            color += letters[Math.round(Math.random() * 15)];
        }
        return color;
    };

    var Tree = function(data, w, h, p, c, cCC) {
        var Tree = this;
        this.color = c || getRandomColor(),
        this.closedCircleColor = cCC || getRandomColor(),
        this.margin = marginSVG,
        this.i = 0;
        this.height = h;
        this.width = w;
        this.branchLength = 100;
        this.leafRadius = minLeafRadius + !(minHeight >= this.height) * Math.abs(Math.tan(1 - minHeight / this.height)) * (maxLeafRadius - minLeafRadius);
        this.position = p;
        this.root = data;
        this.root.x0 = this.height / 2;
        this.root.y0 = 0;

        this.tree = d3.layout.tree()
            .size([this.height - 2 * this.margin, this.width - 2 * this.margin]);

        this.diagonal = d3.svg.diagonal()
            .projection(function(d) {
                return [d.y, d.x];
            });

        this.vis = d3.select('#patterns_server-visu-trees').append('svg:svg')
            .attr('width', this.width)
            .attr('height', this.height)
            .attr('style', 'position:absolute;display:inline;margin-left:' + this.position[0] + 'px; margin-top:' + this.position[1] + 'px;')
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
            // nodes.forEach(function(d) {
            //      // d.y = d.depth * Tree.branchLength;
            // });

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
                })
                .on('mouseover', function(n) {
                    clearTimeout(bubbleTimeOut);
                    bubbleDiv
                        .show()
                        .html('<h3><a href="' + n.pattern_link + '" target="_blank">' + n.name + '</a></h3><p align="justify">Pattern by <b><a href="' + n.d2did_link + '" target="_blank">' + n.author + '</a></b>, in the <i>' + n.category + '</i> category and on <u><a href="' + n.d2did_link + '" target="_blank">this D2D instance</a></u>. </p>');
                }).
            on('mouseout', function() {
                bubbleTimeOut = setTimeout(function() {
                    bubbleDiv.hide();
                }, bubbleShowDuration);
            });

            nodeEnter.append("svg:circle")
                .attr("r", 1e-6)
                .style("fill", function(d) {
                    return d._children ? Tree.closedCircleColor : Tree.color;
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
                .attr("r", Tree.leafRadius)
                .style("fill", function(d) {
                    return d._children ? Tree.closedCircleColor : Tree.color;
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
        build: function(data, location, w, h, isZoomed) {
            var width = w || jQuery(location).width(),
                height = h || jQuery(location).height(),
                nbSquares = Math.ceil(Math.sqrt(data.length));
            // width = width / nbSquares > minWidth ? width / nbSquares : minWidth;
            // height = height / nbSquares > minHeight ? height / nbSquares : minHeight;
            width = width / nbSquares;
            height = height / nbSquares;
            jQuery(location)
                .css({
                    textAlign: 'left',
                    display: 'inline-block',
                    border: 'solid black 1px',
                    overflow: 'hidden'
                })
                .html('<div id="patterns_server-visu-bubble"></div><div id="patterns_server-visu-trees"></div>');
            bubbleDiv = jQuery(location + ' > #patterns_server-visu-bubble');
            bubbleDiv
                .addClass('patterns_server_bubble')
                .css({
                    width: jQuery(location).width() / 2,
                    marginLeft: jQuery(location).width() / 4,
                })
                .on('mouseover', function() {
                    clearTimeout(bubbleTimeOut);
                })
                .on('mouseout', function() {
                    bubbleTimeOut = setTimeout(function() {
                        bubbleDiv.hide();
                    }, bubbleShowDuration);
                });
            svgDiv = jQuery(location + ' > #patterns_server-visu-trees');
            svgDiv.css({
                display: 'inline-block',
                width: width * nbSquares + 'px',
                height: height * nbSquares + 'px',
            })
                .on('mousedown', function() {
                    clearTimeout(bubbleTimeOut);
                    bubbleDiv.hide();
                });
            if (!isZoomed) {
                jQuery(location).bind('mousewheel', d3Tree.detectScroll);
            } else {

            }
            data.forEach(function(el, i, data) {
                var x = (i % nbSquares) * width,
                    y = Math.floor(i / nbSquares) * height;
                if (!trees[i]) {
                    trees[i] = new Tree(el, width, height, [x, y]);
                } else {
                    trees[i] = new Tree(el, width, height, [x, y], trees[i].color, trees[i].closedCircleColor);
                }
            });
            if (height === minHeight || width === minWidth || !! isZoomed) {
                jQuery(location).css({
                    cursor: 'move'
                });
                svgDiv.draggable({
                    drag: function(event, ui) {
                        debugger;
                        var mouseLeft = (event.pageX <= jQuery(location).offset().left),
                            mouseRight = (event.pageX >= jQuery(location).offset().left + jQuery(location).width()),
                            mouseTop = (event.pageY <= jQuery(location).offset().top),
                            mouseBottom = (event.pageY >= jQuery(location).offset().top + jQuery(location).height());
                        if (mouseLeft || mouseRight || mouseTop || mouseBottom) { //Mouse out of the location div
                            return false;
                        }
                        return true;
                    }
                });
            }
        },

        zoomIn: function(location) {
            var curWidth = svgDiv.width(),
                curHeight = svgDiv.height(),
                imageX = svgDiv.offset().left,
                imageY = svgDiv.offset().top,
                newWidth = curWidth * (1 + zoomFactor),
                newHeight = curHeight * (1 + zoomFactor),
                newX = imageX - (curWidth * zoomFactor / 2),
                newY = imageY - (curHeight * zoomFactor / 2);
            d3Tree.build(data, location, newWidth, newHeight, true);
            svgDiv.offset({
                top: newY,
                left: newX,
            });
        },

        zoomOut: function(location) {
            var curWidth = svgDiv.width(),
                curHeight = svgDiv.height(),
                imageX = svgDiv.offset().left,
                imageY = svgDiv.offset().top,
                newWidth = curWidth / (1 + zoomFactor),
                newHeight = curHeight / (1 + zoomFactor),
                newX = imageX + (curWidth * zoomFactor / 2),
                newY = imageY + (curHeight * zoomFactor / 2);
            d3Tree.build(data, location, newWidth, newHeight, true);
            svgDiv.offset({
                top: newY,
                left: newX,
            });
        },

        detectScroll: function(e, delta) {
            if (delta <= 0) {
                d3Tree.zoomOut('#' + this.id);
            } else {
                d3Tree.zoomIn('#' + this.id);
            }
            e.preventDefault();
        },
    };
})();