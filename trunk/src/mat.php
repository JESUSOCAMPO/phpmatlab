<?php
if(!extension_loaded('mat')) {
	dl('mat.so');
}
$module = 'mat';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:<br>\n";
foreach($functions as $func) {
    echo $func."<br>\n";
}
echo "<br>\n";
echo "Testing load: \n";

?>
TODO
