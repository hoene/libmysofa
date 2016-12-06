var diff = require('deep-diff').diff;

if(process.argv.length != 4) {
    console.log("Usage "+process[0]+" "+process[1]+" <LEFT JSON FILE> <RIGHT JSON FILE>\n");
    process.exit(-1);
}
var obj1 = require(process.argv[2]);
var obj2 = require(process.argv[3]);

var res = diff(obj1, obj2);

if(res === undefined) {
    process.exit(0);
}
else {
    console.log("result ",res);
    process.exit(1);
}

