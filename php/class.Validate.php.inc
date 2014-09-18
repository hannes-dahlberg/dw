<?php
namespace dw;

class Validate extends Core{
	public static $validations = [
		'email' => [
			'regex' => '/[a-zA-Z0-9\!\#\$\%\&\'\*\+\-\/\=\?\^\_\`\{\|\}\~\.]{1,64}@[a-zA-Z0-9]{2,250}.[a-zA-Z]{2,10}/',
			'helper' => [
				'min' => 1,
				'max' => 256
			]
		]
		'password' => [
			'regex' => '/^[a-zA-Z0-9\-\_\@\&\.\?\^]{8,20}$/',
			'helper' => [
				'min' => 8,
				'max' => 20,
				'format' => 'a-z, A-Z, 0-9, -_@&.?^'
			]
		]
	];
	
	/*
		Validation of strings
		
		@param string $value The string to be validated
		@param string $validation The key name to use from array $validations. Can be empty if using own validation
		@param multi $regex The regular expression to use (will override any set in validations array)
		@param multi $helper Information about validation (min chars, max chars, format, etc) (will override any set in validation)
		@param bool $match_all If we want to return all matched results or only the first
		
		@return multi Will return array with status code, error code, matched results(s) (if success) or any helper info (if fail). If no regex is submitted and no predefiend regex exists in $validations the function will return true.
	*/
	public static function Validate($value, $validation, $regex = false, $helper = false, $match_all = false) {
		if(isset(self::$validations[$validation]) {
			if (!$regex) {
				$regex = self::$validations[$validation];
			}
			if (!$helper && isset(self::$validations[$validation]['helper'])) {
				$helper = self::$validations[$validation];
			}
		}
		
		if(!$regex) {
			if((!$match_all && $match = preg_match($regex, $value)) || ($match_all && $match = preg_match_all($regex, $value))) {
				$return = [
					'code' => 0,
					'match' => $match
				];
			} else {
				$return = [
					'code' => 1,
					'error' => 1
				];
			}
			if ($helper) {
				$return['helper'] = $helper;
			}
			
			return $return;
		}
		
		return true;
	}
}