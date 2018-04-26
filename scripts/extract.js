const fs = require('fs');
var window = {};
var navigator = {};

var piRenderer = eval('(function() {' +fs.readFileSync('piLibs.js') + 'return piRenderer;})();');
var Effect = eval('(function() {' +fs.readFileSync('effect.js') + 'return Effect;})();');
var EffectPass = eval('(function() {' +fs.readFileSync('effect.js') + 'return EffectPass;})();');

var types = new Array('image', 'buffer', 'common');

for(var i = 0; i < types.length; i++) {
	var ep = new EffectPass(null, true, true, null, null, true, true, null, null, 1);
	ep.mType = types[i];
	ep.MakeHeader();
	fs.mkdirSync(types[i]);
	fs.writeFileSync(types[i]+'/header.glsl', ep.mHeader);
	fs.writeFileSync(types[i]+'/footer.glsl', ep.mImagePassFooter);
}
