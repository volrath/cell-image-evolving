/*
 * JavaScript GA Image Evolver
 * Copyright (c) 2009 Jacob Seidelin, jseidelin@nihilogic.dk, http://blog.nihilogic.dk/
 * MIT License [http://www.nihilogic.dk/licenses/mit-license.txt]
 */

var testWidth = 50;
var testHeight = 50;

var popSize;
var numPolys;
var numPolyPoints;
var mutateChance;
var mutateAmount;
var logField;
var color = true;
var killParents = false;
var parentCutoff;
var uniformCross = false;
var difSquared = true;

var dnaLength;
var dnaCrossLength;

var test, testCtx, out, outCtx;

var $ = function(id) {return document.getElementById(id);};

var testData = [];

Candidate.prototype.calcFitness = function() {
	draw(this.dna, testCtx, 1);

	var data = testCtx.getImageData(0,0,testWidth,testHeight).data;
	var dif = 0;

	var p = testWidth*testHeight*4 - 1;
	do {
		if (p%3) {
			var dp = data[p]-testData[p];
			if (difSquared) {
				dif += dp*dp;
			} else {
				if (dp < 0) 
					dif -= dp;
				else
					dif += dp;
			}
		}
	} while(--p);

	if (difSquared) {
		this.fitness = (1 - dif / (testWidth*testHeight*3*256*256));
	} else {
		this.fitness = (1 - dif / (testWidth*testHeight*3*256));
	}
}

Candidate.prototype.generateRandomDNA = function() {
	var values = [];

	var polyLength = 4 + numPolyPoints * 2;

	for (var i=0;i<dnaLength;i+=polyLength) {

		// rgba
		values.push(Math.random());
		values.push(Math.random());
		values.push(Math.random());
		values.push(Math.max(0.2,Math.random()*Math.random()));

		var px = Math.random();
		var py = Math.random();

		for (var j=0;j<numPolyPoints;j++) {
			values.push(px + (Math.random()-0.5));
			values.push(py + (Math.random()-0.5));
		}
	}

	return values;
}

function draw(dna, ctx, scale) {
	var dw = testWidth * scale;
	var dh = testHeight * scale;
	ctx.fillStyle = "white";
	ctx.fillRect(0,0,dw,dh);

	var vals = dna;

	var polyLength = 4 + numPolyPoints * 2;

	for (var i=0;i<dnaLength;i+=polyLength) {

		ctx.beginPath();
		ctx.moveTo(
			(vals[i+4]) * dw, 
			(vals[i+5]) * dh
		);

		for (var j=0;j<numPolyPoints-1;j++) {
			ctx.lineTo(
				(vals[i+6+j*2]) * dw, 
				(vals[i+7+j*2]) * dh
			);
		}

		ctx.closePath();

		if (numPolys > 1) {
			if (color)
				ctx.fillStyle = "rgba(" + ((vals[i]*255)>>0) + "," + ((vals[i+1]*255)>>0) + "," + ((vals[i+2]*255)>>0) + "," + vals[i+3] + ")";
			else
				ctx.fillStyle = "rgba(" + ((vals[i]*255)>>0) + "," + ((vals[i]*255)>>0) + "," + ((vals[i]*255)>>0) + "," + vals[i+3] + ")";
			ctx.fill();
		} else {
			ctx.lineWidth = 0.5;
			ctx.strokeStyle = "rgba(" + ((vals[i]*255)>>0) + "," + ((vals[i+1]*255)>>0) + "," + ((vals[i+2]*255)>>0) + "," + vals[i+3] + ")";
			ctx.stroke();
		}
	}
}

function log() {
	var args = [];
	for (var i=0;i<arguments.length;i++)
		args.push(arguments[i])
	logField.value = args.join("\r\n");
}

window.onload = function() {
	logField = $("log");
	out = $("output");
	outCtx = out.getContext("2d");

	test = $("test");
	testCtx = test.getContext("2d");

	prepareImage();

	out.width = testWidth*2;
	out.height = testHeight*2;

	out.style.width = "100px";
	out.style.height = "100px";

	test.width = testWidth;
	test.height = testHeight;

	$("startbutton").disabled = false;
}

function prepareImage() {
	var mona = $("mona");
	mona.style.width = "100px";
	mona.style.height = "100px";

	if (!mona.getContext) return;

	var ctx = mona.getContext("2d");
	if (!ctx.getImageData) return;

	mona.width = testWidth;
	mona.height = testHeight;
	ctx.drawImage($("monaimg"),0,0,100,100,0,0,testWidth,testHeight);

	var data = ctx.getImageData(0,0,testWidth,testHeight).data;

	testData = [];
	var p = testWidth*testHeight*4;
	for (var i=0;i<p;i++) {
		testData[i] = data[i];
	}

	mona.width = 100;
	mona.height = 100;
	ctx.drawImage($("monaimg"),0,0);

}

var timer;
var startTime;

function start() {

	var isCapable = true;
	var mona = $("mona");
	if (!mona.getContext) {
		isCapable = false;
		return;
	} else {
		var ctx = mona.getContext("2d");
		if (!ctx.getImageData) {
			isCapable = false;
		}
	}
	if (!isCapable) {
		alert("Please use a canvas enabled browser that supports the getImageData method!");
		return;
	}


	if (timer) {
		clearTimeout(timer);
		timer = 0;
	}

	startTime = new Date().getTime();

	popSize = parseInt($("popsize").value,10);
	numPolys = parseInt($("polynum").value,10);
	numPolyPoints = parseInt($("polypoints").value,10);

	popSize = Math.min(1000,Math.max(10,popSize));
	numPolys = Math.min(100,Math.max(1,numPolys));
	numPolyPoints = Math.min(1000,Math.max(3,numPolyPoints));

	mutateChance = parseFloat($("mutatechance").value);
	mutateAmount = parseFloat($("mutateamount").value);
	mutateChance = Math.min(1,Math.max(0,mutateChance));
	mutateAmount = Math.min(1,Math.max(0,mutateAmount));

	dnaLength = numPolys * (4 + numPolyPoints * 2);
	dnaCrossLength = (4 + numPolyPoints * 2);

	difSquared = $("difsquared").checked;

	uniformCross = $("uniformcross").checked;
	killParents = $("killparents").checked;
	parentCutoff = parseFloat($("parentcutoff").value);
	parentCutoff = Math.max(0.1, Math.min(0.5, parentCutoff));

	var pop = new Population(popSize);
	var genCount = 0;

	function update() {
		pop.nextGeneration();
		var fittest = pop.getFittest();

		var totalTime = ((new Date().getTime() - startTime) / 1000);

		genCount++;
		if (genCount % 10 == 0 || true) {
			outCtx.fillStyle = "black";
			outCtx.fillRect(0,0,testWidth,testHeight);
			draw(fittest.dna, outCtx, 2);
			log(
				"Evolving...", 
				"Polygons: " + numPolys + " (" + (numPolyPoints > 3 ? numPolyPoints + "-gons" : "triangles") + ")",
				"Population size: " + pop.getSize(),
				"Breeding from top " + (parentCutoff*100) + "% of population",
				"Parents " + (killParents ? "killed off after breeding" : "carried over to next generation"),
				"---",
				"Generation: " + genCount, 
				"Best fit: " + (fittest.fitness * 100).toFixed(4) + "%",
				"---",
				"Time: " + totalTime.toFixed(2),
				"Time per generation: " + (totalTime / genCount).toFixed(2)
		
			);
		}

		timer = setTimeout(update,10);
	}

	update();

}

function stop() {
	clearTimeout(timer);
}

function changeImage() {
	var select = $("evolveimage");
	var src = select.options[select.selectedIndex].value;
	$("monaimg").onload = prepareImage;
	$("monaimg").src = src;

	color = !(src == "monalisa_gray_100x100.png");
}
