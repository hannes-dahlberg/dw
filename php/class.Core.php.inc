<?php
namespace dw;

class Core extends GetSet{
	protected static $_coreMySQLConnection = false;
	protected static $_admin = false;
	protected static $_language = 'sv';
	
	public static function Pre($object) {
		echo '<pre>'. print_r($object, true). '</pre>';
	}
	
	public static function PreDie($object) {
		self::pre($object);
		die();
	}
	
	public static function SetMySQLConnection($mysql_connection) {
		self::$_coreMySQLConnection = $mysql_connection;
	}
	
	public static function SetAdmin($value = false) {
		if($value) {
			return self::$_admin = true;
		}
		
		return self::$_admin = false;
	}
	
	public static function SetLanguage($language) {
		$language = strtolower($language);
		
		if(!self::$_coreMySQLConnection) {
			return false;
		}
		
		if($language==self::$_language) {
			return true;
		}
		
		if(!empty(self::$_coreMySQLConnection->FetchField('SELECT `language` FROM `languages` WHERE `language` = \''. $language. '\''))) {
			self::$_language = $language;
		}
		
	}
	
	public static function InstallLanguageTables($prefix = '', $languages = ['en', 'sv']) {
		if(!self::$_coreMySQLConnection) {
			return false;
		}
		
		self::$_coreMySQLConnection->Query('
			CREATE TABLE IF NOT EXISTS `'. $prefix. 'languages` (
				`language_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
				`language` varchar(2) DEFAULT NULL,
				`name` int(11) DEFAULT NULL,
				`description` text,
				PRIMARY KEY (`language_id`)
			) ENGINE=InnoDB DEFAULT CHARSET=utf8;
		');
		self::$_coreMySQLConnection->Query('	
			CREATE TABLE IF NOT EXISTS `'. $prefix. 'translation_values` (
				`translation_value_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
				`translation_id` int(11) DEFAULT NULL,
				`language_id` int(11) DEFAULT NULL,
				`translation_value` text,
				PRIMARY KEY (`translation_value_id`)
			) ENGINE=InnoDB DEFAULT CHARSET=utf8;
		');
		self::$_coreMySQLConnection->Query('
			CREATE TABLE IF NOT EXISTS `'. $prefix. 'translations` (
				`translation_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
				`translation` varchar(255) DEFAULT NULL,
				PRIMARY KEY (`translation_id`)
			) ENGINE=InnoDB DEFAULT CHARSET=utf8;
		');
		
		if(!empty($languages)) {
			foreach($languages as $language) {
				if(empty(self::$_coreMySQLConnection->FetchField('SELECT `language` FROM `languages` WHERE `language` = \''. $language. '\' LIMIT 1'))) {
					self::$_coreMySQLConnection->Query('INSERT INTO `'. $prefix. 'languages` (`language`) VALUES(\''. $language. '\')');	
				}
			}
		}
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
}