--TEST--
phpinfo() shows viaeval info
--EXTENSIONS--
viaeval
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
if (strpos($info, 'Eval') !== false && strpos($info, 'Loaded') !== false) {
    echo 'info found';
} else {
    echo 'info missing';
}
?>
--EXPECT--
info found
