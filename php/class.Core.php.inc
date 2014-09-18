<?php
namespace dw;

class Core extends GetSet{
	protected static $_coreMySQLConnection = false; //This will hold any mysql connection to be used through out the dw library
	protected static $_admin = false; //Set to true and error messages will print, false makes all error silent
	protected static $_language = 'sv'; //The language to use
	
	/*
		Print objects or arrays in a friendly matter. This method will not return anything, the object
		will be printed as the method is called
		
		@param mixed $object The object to be print
	*/
	public static function Pre($object) {
		echo '<pre>'. print_r($object, true). '</pre>';
	}
	
	/*
		Short hand method for above method when you want to print out object in a friendly matter and
		stop script right after.
		
		@param mixed $object The object to be frinted
	*/
	public static function PreDie($object) {
		//Calling the original method for printing friendly
		self::pre($object);
		die(); //Die right after
	}
	
	/*
		Method to implode with key and values as part of the glue. This is good for example when putting an
		array into an SQL UPDATE statement: ['key1' => 'value1', 'key2' => 'value2'] = '`key1` = "value1", `key2` = "value2"'
		Example to use it on an array to make it SQL UPDATE is:
		ImplodeWithKey(', ', '%KEY% = %VALUE%', ['field_name' => 'value']);
		
		@param string $glue What to bind each array item to each other. Works just like the glue in a regular implode
		@param string $replace How to use key and value when putting the array to string
		@param array $array The array to implode
		
		@return string The glued array
	*/
	public static function ImplodeWithKey($glue, $replace, $array) {
		$return = '';
		$counter = 1;
		$length = count($array);
		foreach($array as $key => $value) {
			$return .= str_replace(['%KEY%', '%VALUE%'], [$key, $value], $replace). ($counter < $length) ? $glue : '';
			
			$counter++;
		}
		
		return $return;
	}
	
	/*
		Set the connection for the static protected parameter. Suggest to use this method the first thing
		you do after including the library. Ofcourse you need to declare the mysql connection first by creating
		a dw\MySQL object
		
		@param dw\MySQL $mysqlConnection The SQL connection to use
		
		@return bool Will return true if nothing happens and die before
	*/
	public static function SetMySQLConnection($mysqlConnection) {
		self::$_coreMySQLConnection = $mysqlConnection;
		
		return true;
	}
	
	/*
		Set to admin and errors will print for all dw library methods
		
		@param bool $value Whatever value to set it to
		
		@return bool return true if nothing else
	*/
	public static function SetAdmin($value = false) {
		self::$_admin = $value ? true : false;
		
		return true;
	}
	
	/*
		Set language to use. Will check if the actual language exists before setting. This method require you to install
		the language tables before execution
		
		@param string $language The language to set to
		
		@return bool True if set was successfull and language exists, otherwise false
	*/
	public static function SetLanguage($language) {
		//Set to lowercase
		$language = strtolower($language);
		
		//Check if an sql connection exists, if not return false
		if(!self::$_coreMySQLConnection) {
			return false;
		}
		
		//If trying to set to the language already set
		if($language == self::$_language) {
			return true;
		}
		
		//Check if language exists in db
		if(!empty(self::$_coreMySQLConnection->FetchField('SELECT `language` FROM `languages` WHERE `language` = \''. $language. '\''))) {
			//Set to the new language and return true
			self::$_language = $language;
			return true;
		}
		
		//Nothing worked
		return false;
	}
	
	/*
		Installing language and translations tables. Good for use when you want translations for your website
		
		@param array $languages The languages you want to have in the database from the beginning, needs to
		be an array
		
		@return bool true if all goes well, otherwise false
	*/
	public static function InstallLanguageTables($languages = ['en', 'sv']) {
		//If no sql connection exists, return false
		if(!self::$_coreMySQLConnection) {
			return false;
		}
		
		//Creates language table
		self::$_coreMySQLConnection->Query('
			CREATE TABLE IF NOT EXISTS `languages` (
				`language_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
				`language` varchar(2) DEFAULT NULL,
				`name` int(11) DEFAULT NULL,
				`description` text,
				PRIMARY KEY (`language_id`)
			) ENGINE=InnoDB DEFAULT CHARSET=utf8;
		');
		
		//creates table for translation values
		self::$_coreMySQLConnection->Query('	
			CREATE TABLE IF NOT EXISTS `translation_values` (
				`translation_value_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
				`translation_id` int(11) DEFAULT NULL,
				`language_id` int(11) DEFAULT NULL,
				`translation_value` text,
				PRIMARY KEY (`translation_value_id`)
			) ENGINE=InnoDB DEFAULT CHARSET=utf8;
		');
		
		//Create table for translations names
		self::$_coreMySQLConnection->Query('
			CREATE TABLE IF NOT EXISTS `translations` (
				`translation_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
				`translation` varchar(255) DEFAULT NULL,
				PRIMARY KEY (`translation_id`)
			) ENGINE=InnoDB DEFAULT CHARSET=utf8;
		');
		
		//Create view for translations values connected to translation names
		self::$_coreMySQLConnection->Query('
			CREATE VIEW `translations_view` AS
				SELECT
					`translations`.`translation` AS `translation`,
					`translation_values`.`translation_value` AS `translation_value`,
					`languages`.`language` AS `language`
				FROM
					`translation_values`
				INNER JOIN
					`translations` ON
						`translation_values`.`translation_id` = `translations`.`translation_id`
				INNER JOIN
					`languages` ON
						`translation_values`.`language_id` = `languages`.`language_id`
		');
		
		//If any pre-defined languages is set in parameter, insters them to the language table
		if(!empty($languages)) {
			//Going through all languages in the array
			foreach($languages as $language) {
				//Checking if not already exists
				if(empty(self::$_coreMySQLConnection->FetchField('SELECT `language` FROM `languages` WHERE `language` = \''. $language. '\' LIMIT 1'))) {
					self::$_coreMySQLConnection->Query('INSERT INTO `languages` (`language`) VALUES(\''. $language. '\')');	
				}
			}
		}
		
		return true;
	}
	
	public static function __($language_variable, $language = false) {
		if(!$language) {
			$language = self::$_language;
		}
		
		if(!self::$_coreMySQLConnection) {
			return false;
		}
		return self::$_coreMySQLConnection->FetchField('
			SELECT
				IFNULL
				(
					(
						SELECT
							`translation_value`
						FROM
							`translations_view`
						WHERE
							`language` = \''. $language. '\' AND
							`translation` = \''. $language_variable. '\'
						LIMIT 1
					),
					\'_'. $language_variable. '\'
				)
		');
	}
	
	
	public static function AddLanguageVariable($language_variable, $translation_value, $language = false) {
		if(!$language) {
			$language = self::$_language;
		}
		
		if(!self::$_coreMySQLConnection) {
			return false;
		}
		$language_id = self::$_coreMySQLConnection->FetchField('SELECT `language_id` FROM `languages` WHERE `language` = \''. $language. '\' LIMIT 1');
		$translation_id = self::$_coreMySQLConnection->FetchField('SELECT `translation_id` FROM `translations` WHERE `translation` = \''. $language_variable. '\' LIMIT 1');
		
		if(empty($language_id)) {
			return false;
		}
		
		$translation_value_id = '';
		if(!empty($translation_id)) {
			$translation_value_id = self::$_coreMySQLConnection->FetchField('SELECT `translation_value_id` FROM `translations_view` WHERE `translation` = \''. $language_variable. '\' AND `language` = \''. $language. '\' LIMIT 1');
		}
		
		if (empty($translation_value_id)) {
			$translation_id = self::$_coreMySQLConnection->Query('INSERT INTO `translations` (`translation`) VALUES(\''. $language_variable. '\')');
			self::$_coreMySQLConnection->Query('INSERT INTO `translation_values` (`translation_id`, `language_id`, `translation_value`) VALUES(\''. $translation_id. '\', \''. $language_id. '\', \''. $translation_value. '\')');
		} else {
			self::$_coreMySQLConnection->Query('UPDATE `translation_values` SET `translation_value` = \''. $translation_value. '\' WHERE `translation_value_id` = \''. $translation_value_id. '\'');
		}
	}
	
	/*
		This is basically used for installing user related tables to any system using the dw library
		
		@return bool If no errors occure during the run of sql statements a true will be returned at the end
	*/
	public static function InstallUserTables() {
		//Install users table
		self::$_coreMySQLConnection->Query('
			CREATE TABLE IF NOT EXISTS `users` (
				`user_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
				`email` varchar(255) DEFAULT NULL,
				`password` varchar(255) DEFAULT NULL,
				`first_name` varchar(20) DEFAULT NULL,
				`last_name` varchar(20) DEFAULT NULL,
				`type` varchar(1) DEFAULT NULL,
				`registered` datetime DEFAULT NULL,
				`last_update` datetime DEFAULT NULL,
				PRIMARY KEY (`user_id`)
			) ENGINE=InnoDB DEFAULT CHARSET=utf8;
		');
		
		//Install groups table
		self::$_coreMySQLConnection->Query('
			CREATE TABLE NOT EXISTS `groups` (
				`group_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
				`name` varchar(20) DEFAULT NULL,
				`title` varchar(255) DEFAULT NULL,
				`description` varchar(255) DEFAULT NULL,
				PRIMARY KEY (`group_id`)
			) ENGINE=InnoDB DEFAULT CHARSET=utf8;
		');
		
		//Install ban table
		self::$_coreMySQLConnection->Query('
			CREATE TABLE NOT EXISTS `groups` (
				`id` int(11) unsigned NOT NULL AUTO_INCREMENT,
				`ip` varchar(15) DEFAULT NULL,
				`email` varchar(255) DEFAULT NULL,
				`string` varchar(255) DEFAULT NULL,
				PRIMARY KEY (`id`)
			) ENGINE=InnoDB DEFAULT CHARSET=utf8;
		');
		
		return true;
	}
}