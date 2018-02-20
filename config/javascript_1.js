// http://codeforces.com/contest/915/submission/34156623

var aOrig = readline();
var bOrig = readline();
var bIsLonger = false;
var prevIsBigger = false;


if (aOrig.length < bOrig.length) {
    bOrig = bOrig.substr(0, aOrig.length);
    bIsLonger = true;
}

var a = aOrig.split('');
var b = bOrig.split('');

var map = {
    0: 0,
    1: 0,
    2: 0,
    3: 0,
    4: 0,
    5: 0,
    6: 0,
    7: 0,
    8: 0,
    9: 0
};

var result = '';

function findMax(e) {
    var origE = e;
    if (bIsLonger || typeof prevIsBigger === 'number') {
        e = 9;
    }
    while(e > -1) {
        if (map[e]) {
            map[e]--;
            return e;
        }
        e--;
        if (prevIsBigger === false) {
            prevIsBigger = result.length;
        }
    }
    var prev = result[result.length - 1];
    map[prev]++;
    if (prevIsBigger === result.length) {
        prevIsBigger = false;
    }
    result = result.substring(0, result.length - 1);
    var q = findMax(prev - 1);
    result += q;
    prevIsBigger = result.length;
    return findMax(origE);

}

a.forEach(e => {
    map[e]++;
});
b.forEach(e => {
    var c = findMax(e);
    result += c;
});

print(result);
