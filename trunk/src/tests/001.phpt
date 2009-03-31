--TEST--
Check for phpmat presence
--SKIPIF--
<?php if (!extension_loaded("mat")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
echo "mat extension is available";
?>
--EXPECT--
mat extension is available
