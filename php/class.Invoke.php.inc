<?php
namespace dw;

class Invoke {
	static public $php = [
		'class.GetSet.php.inc',
		'class.Core.php.inc',
		'class.Ban.php.inc',
		'class.Error.php.inc',
		'class.Group.php.inc',
		'class.Groups.php.inc',
		'class.MySQL.php.inc',
		'class.User.php.inc',
		'class.Users.php.inc',
		'class.Validate.php.inc'
	];
	static private $js = [
		'dw.js'
	];
	
	static public function IncludeJs($root) {
		$include_rows = '';	
		foreach(self::$js as $js) {
			$include_rows .= '<script src="'. $root. 'js/'. $js. '"></script>'. "\n";
		}
		
		return $include_rows;
	}
}