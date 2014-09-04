<?php
include_once('php/class.Invoke.php.inc');

foreach(dw\Invoke::$php as $php) {
	if(file_exists(__DIR__. '/php/'. $php)) {
		include_once(__DIR__. '/php/'. $php);
	}
}