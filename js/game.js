function Game() {
	var canvas = this.getCanvas();
	var context = canvas.getContext("2d");
	
	this.mouseX = this.mouseY = 0;
	this.gridX = this.gridY = -1;
	this.gridWall = true;
	
	this.jumpDown = false;
	this.leftDown = false;
	this.rightDown = false;
	
	// Create a grid with a floor over its entire width
	this.grid = new PlatformerGrid(
		Math.floor(canvas.width / this.GRID_RESOLUTION),
		Math.floor(canvas.height / this.GRID_RESOLUTION),
		this.GRID_RESOLUTION);
		
	for(var x = 0; x < this.grid.width; ++x)
		this.grid.setCeiling(x, this.grid.height - 1, true);
	
	// Create a player
	this.player = new PlatformerNode(100, 100, this.PLAYER_SIZE, this.PLAYER_SIZE);
	this.grid.addNode(this.player);
	
	this.addListeners();
};

Game.prototype = {
	GRID_RESOLUTION: 32,
	PLAYER_SIZE: 24,
	PAINT_STROKE_STYLE: "lime",
	PAINT_LINE_WIDTH: 3,
	ERASE_STROKE_STYLE: "red",
	ERASE_LINE_WIDTH: 3,
	PLAYER_JUMP_SPEED: -800,
	PLAYER_WALK_SPEED: 270,
	PLAYER_WALK_ACCELERATION: 2500,
	KEY_JUMP: 87,
	KEY_LEFT: 65,
	KEY_RIGHT: 68,
	
	addListeners() {
		this.getCanvas().addEventListener("click", this.mouseClick.bind(this));
		this.getCanvas().addEventListener("mousemove", this.mouseMove.bind(this));
		this.getCanvas().addEventListener("mouseout", this.mouseLeave.bind(this));
		
		window.addEventListener("keydown", this.keyDown.bind(this));
		window.addEventListener("keyup", this.keyUp.bind(this));
	},

	getCanvas() {
		return document.getElementById("renderer");
	},

	run() {
		this.lastTime = new Date();
		
		window.requestAnimationFrame(this.animate.bind(this));
	},

	keyDown(e) {
		switch(e.keyCode) {
			case this.KEY_JUMP:
				if(!this.jumpDown && this.player.onGround) {
					this.jumpDown = true;
					this.player.setvy(this.PLAYER_JUMP_SPEED);
				}
				
				break;
			case this.KEY_RIGHT:
				this.rightDown = true;				
				break;
			case this.KEY_LEFT:
				this.leftDown = true;
				break;
		}
	},

	keyUp(e) {
		switch(e.keyCode) {
			case this.KEY_JUMP:
				this.jumpDown = false;
				break;
			case this.KEY_RIGHT:
				this.rightDown = false;
				break;
			case this.KEY_LEFT:
				this.leftDown = false;
				break;
		}
	},

	mouseClick(e) {
		if(this.gridX == -1 || this.gridY == -1)
			return;
		
		// Toggle selected edge
		if(this.gridWall)
			this.grid.setWall(this.gridX, this.gridY, !this.grid.getWall(this.gridX, this.gridY));
		else
			this.grid.setCeiling(this.gridX, this.gridY, !this.grid.getCeiling(this.gridX, this.gridY));
	},

	mouseMove(e) {
		const bounds = this.getCanvas().getBoundingClientRect();
		
		// Store mouse coordinates
		this.mouseX = e.clientX - bounds.left;
		this.mouseY = e.clientY - bounds.top;
		
		// Store current grid cell
		this.gridX = Math.floor(this.mouseX / this.GRID_RESOLUTION);
		this.gridY = Math.floor(this.mouseY / this.GRID_RESOLUTION);
		
		// Determine selected edge
		const deltaX = this.mouseX - this.gridX * this.GRID_RESOLUTION;
		const deltaY = this.mouseY - this.gridY * this.GRID_RESOLUTION;
		this.gridWall = deltaX * deltaX < deltaY * deltaY;
		
		if(deltaX + deltaY > this.GRID_RESOLUTION) {
			if(deltaX > deltaY) {
				this.gridX = Math.min(this.gridX + 1, this.grid.width);
			}
			else {
				this.gridY = Math.min(this.gridY + 1, this.grid.height);
			}
			
			this.gridWall = !this.gridWall;
		}
	},

	mouseLeave(e) {
		this.gridX = this.gridY = -1;
	},

	animate() {
		var time = new Date();
		var timeStep = (time.getMilliseconds() - this.lastTime.getMilliseconds()) / 1000;
		if(timeStep < 0)
			timeStep += 1;
		
		this.lastTime = time;
		
		this.movePlayer(timeStep);
		this.grid.update(timeStep);
		this.render(timeStep);
		
		window.requestAnimationFrame(this.animate.bind(this));
	},
	
	movePlayer(timeStep) {
		if(this.rightDown) {
			this.player.setvx(Math.min(this.player.vx + this.PLAYER_WALK_ACCELERATION * timeStep, this.PLAYER_WALK_SPEED));
		}
		
		if(this.leftDown) {
			this.player.setvx(Math.max(this.player.vx - this.PLAYER_WALK_ACCELERATION * timeStep, -this.PLAYER_WALK_SPEED));
		}
	},

	render(timeStep) {
		var canvas = this.getCanvas();
		var context = canvas.getContext("2d");
		
		// Clear canvas
		context.clearRect(0, 0, canvas.width, canvas.height);
		this.grid.draw(context);
		
		// Draw selected edge
		if(this.gridX != -1 && this.gridY != -1) {
			context.beginPath();
			
			if(this.gridWall) {
				if(this.grid.getWall(this.gridX, this.gridY)) {
					context.strokeStyle = this.ERASE_STROKE_STYLE;
					context.lineWidth = this.ERASE_LINE_WIDTH;
				}
				else {
					context.strokeStyle = this.PAINT_STROKE_STYLE;
					context.lineWidth = this.PAINT_LINE_WIDTH;
				}
				
				context.moveTo(this.gridX * this.GRID_RESOLUTION, this.gridY * this.GRID_RESOLUTION);
				context.lineTo(this.gridX * this.GRID_RESOLUTION, (this.gridY + 1) * this.GRID_RESOLUTION);
			}
			else {
				if(this.grid.getCeiling(this.gridX, this.gridY)) {
					context.strokeStyle = this.ERASE_STROKE_STYLE;
					context.lineWidth = this.ERASE_LINE_WIDTH;
				}
				else {
					context.strokeStyle = this.PAINT_STROKE_STYLE;
					context.lineWidth = this.PAINT_LINE_WIDTH;
				}
				
				context.moveTo(this.gridX * this.GRID_RESOLUTION, this.gridY * this.GRID_RESOLUTION);
				context.lineTo((this.gridX + 1) * this.GRID_RESOLUTION, this.gridY * this.GRID_RESOLUTION);
			}
			
			context.stroke();
		}
	}
};