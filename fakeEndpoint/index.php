<?php

$file = 'status.json';

if (!isset($_POST['key'])) {
    die('no key!');
}

if (isset($_POST['key']) && $_POST['key'] !== 'YOUR_KEY_HERE') {
    die('invalid key!');
}

$data = json_decode(file_get_contents($file), true);

if (isset($_POST['sensors'])) {
    $sensors = json_decode($_POST['sensors'], true);
    $data['sensors'] = $sensors['sensors'];
    echo "sensors set!\n";
}

if (isset($_POST['temp'])) {
    $temp = floatval($_POST['temp']);
    $data['sensors']['temperature'][0]['value'] = $temp;
    echo "temp set to $temp!\n";
}

if (isset($_POST['door'])) {
    $door = $_POST['door'] == "null" ? null : $_POST['door'] == "true";
    $data['state']['open'] = $door;
    echo "door set to $door!\n";
}

// Write the contents back to the file
file_put_contents($file, json_encode($data, JSON_PRETTY_PRINT));

echo "thanks!";
?>


