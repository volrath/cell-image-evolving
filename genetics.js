/*
 * JavaScript Genetic Algorithm
 * Copyright (c) 2009 Jacob Seidelin, jseidelin@nihilogic.dk, http://blog.nihilogic.dk/
 * MIT License [http://www.nihilogic.dk/licenses/mit-license.txt]
 */

function Population(size) {

	var candidates = [];

	for (var i=0;i<size;i++) {
		candidates.push(new Candidate());
	}

	this.nextGeneration = function(mutateChance, mutateAmount) {
		if (candidates.length > 1) {
			candidates = candidates.sort(
				function(a, b) {
					return b.fitness - a.fitness;
				}
			)

			var offspring = [];

			
			var numCoolParents = Math.floor(candidates.length * parentCutoff);
			var numChildren = Math.ceil(1 / parentCutoff);
			if (!killParents)
				numChildren--;

			for (var i=0;i<numCoolParents;i++) {
				var dna = candidates[i].dna;
				for (var j=0;j<numChildren;j++) {
					var rndCandidate = i;
					while (rndCandidate == i)
						rndCandidate = (Math.random() * numCoolParents)>>0;

					offspring.push(
						new Candidate(dna, candidates[rndCandidate].dna)
					);
				}
			}
			
			if (killParents) {
				candidates = offspring;
			} else {
				candidates.length = numCoolParents;
				candidates = candidates.concat(offspring);
			}
			candidates.length = size;
		} else {
			var parent = candidates[0];
			var child = new Candidate(parent.dna, parent.dna);
			if (child.fitness < candidates[0].fitness) {
				candidates = [child];
			}
		}
	}

	this.getSize = function() {
		return candidates.length;
	}

	this.getFittest = function() {
		return candidates.sort(
			function(a, b) {
				return b.fitness - a.fitness;
			}
		)[0];
	}
}

function Candidate(parDna1, parDna2) {

	var values = [];

	if (parDna1 && parDna2) {
		var cross = (Math.random() * dnaLength)>>0;

		for (var i=0;i<dnaLength;i+=dnaCrossLength) {
			var val;
			var par;
			if (uniformCross) {
				if (Math.random() < 0.5) {
					par = parDna1;
				} else {
					par = parDna2;
				}
			} else {
				if (i<cross) {
					par = parDna1;
				} else {
					par = parDna2;
				}
			}
			for (var j=0;j<dnaCrossLength;j++) {
				val = par[i+j];
				if (Math.random() < mutateChance) {
					val += Math.random() * mutateAmount * 2 - mutateAmount;

					if (val < 0) val = 0;
					if (val > 1) val = 1;
				}
				values.push(val);
			}
		}
	} else {
		values = this.generateRandomDNA(dnaLength);
	}

	this.dna = values;
	this.fitness = 0;

	this.calcFitness()
}

Candidate.prototype.generateRandomDNA = function() {
	var values = [];
	for (var i=0;i<dnaLength;i++)
		values[i] = Math.random();
	return values;
}