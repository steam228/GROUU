var cnv;
var MouseOldX;
var MouseOldY;
var red;
var blue;
const vari = 80;
var base
var toogle;


function setup() {
  createCanvas(windowWidth, windowHeight);
  //cnv.parent('body');

  MouseOldX = 0;
  MouseOldY = 0;
  red = 0 ;
  blue = 0;
  base = 255-vari;
  toogle = true;
}

function draw() {

if (toogle==true){
if (abs(mouseX-MouseOldX)  > 10 || abs(mouseY-MouseOldY)  > 10 ){
red = map(mouseX, 0, width,-vari,+vari); 
blue = map(mouseY, 0, height,-vari,+vari);
background(vari, base+red,base+blue);
MouseOldX = mouseX;
MouseOldY = mouseY;
}
}
  
  
}


function windowResized() {
  resizeCanvas(windowWidth, windowHeight);
}

function mouseClicked() {
  toogle = !toogle;
}



