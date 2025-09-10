--TEST--
Eval disabled
--EXTENSIONS--
viaeval
--FILE--
<?php
eval('1+1;');
?>
--EXPECTF--
Fatal error: %s
