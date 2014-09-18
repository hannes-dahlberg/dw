<?php
namespace dw;

class User extends Core {
	public $email;
	private $_password;
	public $firstName;
	public $lastName;
	private $_type;
	private $_status;
	private $_groups = [];
	private $_registered;
	private $_lastUpdate;
	
	public function __construct($params) {
		if(isset($params['user_id'])) {
			$user = self::$_coreMySQLConnection->FetchRow('SELECT * FROM `users` WHERE `user_id` = \''. intval($params['user_id']). '\' LIMIT 1');
			
			if (!empty($user['user_id'])) {
				$this->AssignProperties($user);
			} else {
				return new dw\Error('W', 'User not found', 'The user was not found');
			}
		} else {
			
		}
	}
	
	/*
		We want to have the ability to extend this function. Therefore this method analyzes every existing property before assigning. As long
		as there's a property responding to the field name from the db-result it will be assign (private or public). Important to know is that
		properties need to be named with prefix underscore for private and use camelCase naming without leading captial. The fields in the db
		need to be written with snake_case.
		
		@param array $array keys needs to represent class properties using snake_case lettering
	*/
	private function AssignProperties($array) {
		foreach($array as $field => $value) {
			$publicVarName = preg_replace('/(.+)_(.)(.+)/e', '"$1".strtoupper("$2")."$3"', $field);
			$privateVarName = '_'. $publicVarName;
			if (isset($this->$publicVarName)) {
				$this->$publicVarName = $value;
			} elseif (isset($this->$privateVarName)) {
				$this->$privateVarName = $value;
			}
		}
	}
	
	/*
		This function is for adding users to the DB. All fields will be validated if the exists as validators in the class dw\Validate and skip_validation is not set or false
		For each parameter a validate field can be passed. This will tell what type of validation is to be done on the field instead of using the same name as the db field name.
		The input needs to be structured as such:
		[
			[DB_FIELD] => 'DB_VALUE' OR [
				'value' => 'DB_VALUE',
				'validate' => 'field to validate', //will use DB_FIELD if not set
				'validation_result' => [] //This will be set after validation is run and if any validation error occure all params will be return
			]
		]
		
		@param array $params All data and information about the value
		
		@return mixed True if user was added succesfully or params with validation result for each field
	*/
	public static function AddUser($params, $skip_validation = false) {
		$validation_error = false; //Sets to true as soon any validation error occure
		if(!$skip_validation) { //Param $skip_validate can be set to true to skip any typ of validation
			foreach($params as $key => &$param) { //Go through all data values set as params
				if(!is_array($param)) { //Fix value of param to an array
					$param = ['value' => $param];
				}
				
				if(!isset($param['validate'])) { //Set 
					$param['validate'] = $key;
				}
				
				if((isset(dw\Validate::$validations[$param['validate']])) {
					$param['validation_result'] = !dw\Validate::Validate($param['value'], $param['validate'])
					if($param['validation_result']['error']!=0 && !$validation_error) {
						$validation_error = true;
					}
				}
			}
		}
		if ($validation_error) {
			return $params;
		}
		
		//Validation was succesfull, now lets check if the user already exists
		
		self::$_coreMySQLConnection->ReplaceInto('users', $params);
		
		return true;
	}
}