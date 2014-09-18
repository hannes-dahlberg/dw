<?php
namespace dw;

class MySQL extends Core{
	private $_connection;
	
	private static $dataTypes = [
		'%i' => 'int',
		'%d' => 'double',
		'%s' => 'string',
		'%wa' => 'where_and',
		'%wo' => 'where_or'
	];
	
	public function __construct($host = 'localhost', $username = 'root', $password = false, $database = false) {
		$this->_connection = new \mysqli($host, $username, $password, $database);
		$this->_connection->set_charset('utf8');
	}
	
	
	public function FetchField($sql, $data = false) {
		
		$result = $this->Query($sql, $data)->fetch_array();
		if (is_array($result)) {
			return array_shift($result);
		}
		
		return false;
	}
	
	public function FetchRow($sql, $data) {
		return $this->Query($sql, $data)->fetch_assoc();
	}
	
	public function FetchRows($sql, $hashKey = false, $data = false) {
		$result = $this->Query($sql, $data);
	
		$rows = array();
		while($row = $result->fetch_assoc()) { //Go through all rows of the result
			//Set hash key to false if field doesn't exists
			if(!$hashKey && !isset($row[$hashKey])) { //Second statement will not be excecuted if hash key is false so we don't need to check for the same field on every row
				$hashKey = false;
			}
			
			//Add row to result array ($rows)
			if ($hashKey) { //Check if hash key is still not false (means the field exists) and use that fields value as key for the array
				$rows[$row[$hashKey]] = $row;
			} else {
				$rows[] = $row;
			}
		}
		
		return $rows;
	}
	
	public function NumRows($result) {
		return $result->num_rows;
	}
	
	/*
		The core method of this class. This is used to run sql queries and can be used with some modifications.
		The data variable can be filled with different values for 
	*/
	public function Query($sql, $data = false) {
		$output = [];
		preg_match_all("/(%i|%s|%d|%l)/", $sql, $output);
		foreach($output as $key => $item) {
			switch(self::$dataTypes[$item]) {
				case 'int':
					$data[$key] = intval($data[$key]);
					break;
				case 'double':
					$data[$key] = floatval($data[$key]);
					break;
				case 'where_and':
					$data[$key] = '(\''. implode('\' AND \'', ). '\')';
					break;
				case 'where_or':
					$data[$key] = '(\''. implode('\' OR \'', ). '\')';
					break;
				case 'string':
				default:
					$data[$key] = '\''. $data[$key]. '\''
			}
		}
		
		$sql = preg_replace('/(%i|%s|%d|%l)/', $data, $subject);
		
		if(!$result = $this->_connection->query($sql)){
			if(self::$_admin) {
				self::PreDie(['SQL_ERROR' => ['QUERY' => $sql, 'ERROR' => $this->_connection->error]]);
			} else {
				die('Error!!!');
			}
		}
		
		if(strpos($sql,'INSERT') !== false) {
			return $this->_connection->insert_id;
		}
		
		return $result;
	}
	
	public function MultiQuery($sql) {
		$this->multi_query($sql);
	}
	
	public function ReplaceInto($table, $data, $insert = false) {
		foreach($data as $key => $value) {
			$fields = '`'. $key. '`, ';
			if (is_array($value) && isset($value['value'])) { //This makes it possible for each data value to be an array where value is defined with key "value"
				$value = $value['value'];
			}
			$values = '\''. $value. '\', ';
		}
		$fields = substr($fields, 0, -2);
		$values = substr($value, 0, -2);
		
		$insertSql = 'REPLACE';
		if($insert) {
			$insertSql = 'INSERT';
		}
		$this->Query($insert_sql. ' INTO `'. $table. '` ('. $fields. ') VALUES('. $values. ')');
	}
	
	public function InsertInto($table, $data) {
		ReplaceInto($table, $data, true);
	}
	
	public function Update($table, $data) {
		$sqlUpdate = '';
		foreach($data as $key => $value) {
			$sqlUpdate .= ``
		}
	}
}