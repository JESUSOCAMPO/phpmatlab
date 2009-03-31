--TEST--
Check loading parsing
--SKIPIF--
<?php if (!extension_loaded("mat")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
var_dump(load_mat('test.mat'));
?>
--EXPECT--
