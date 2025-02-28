<?php
<?php
$array = ["apple", "banana", "cherry"];
foreach ($array as $value) {
    echo $value;
    echo "\n";
}

$assoc = ["a" => "apple", "b" => "banana"];
foreach ($assoc as $key => $value) {
    echo $key;
    echo ": ";
    echo $value;
    echo "\n";
}
$x = 10;
$y = $x + 5;
$z = $y * 2;
$w = $z / 4;
$v = $w - 1;
echo "x: ";
echo $x;
echo "y: ";
echo $y;
echo "z: ";
echo $z;
echo "w: ";
echo $w;
echo "v: ";
echo $v;