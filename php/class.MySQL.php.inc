<?php
namespace dw;

class MySQL extends Core{
	private $_connection;
	
	public function __construct($host = 'localhost', $username = 'root', $password = false, $database = false) {
		$this->_connection = new \mysqli($host, $username, $password, $database);
		$this->_connection->set_charset('utf8');
	}
	
	
	public function FetchField($sql) {
		
		$result = $this->Query($sql)->fetch_array();
		if (is_array($result)) {
			return array_shift($result);
		}
		
		return false;
	}
	
	public function FetchRow($sql) {
		return $this->Query($sql)->fetch_assoc();
	}
	
	public function FetchRows($sql, $hash_key = false) {
		$result = $this->Query($sql);
	
		$rows = array();
		while($row = $result->fetch_assoc()) { //Go through all rows of the result
			//Set hash key to false if field doesn't exists
			if(!$hash_key && !isset($row[$hash_key])) { //Second statement will not be excecuted if hash key is false so we don't need to check for the same field on every row
				$hash_key = false;
			}
			
			//Add row to result array ($rows)
			if ($hash_key) { //Check if hash key is still not false (means the field exists) and use that fields value as key for the array
				$rows[$row[$hash_key]] = $row;
			} else {
				$rows[] = $row;
			}
		}
		
		return $rows;
	}
	
	public function NumRows($result) {
		return $result->num_rows;
	}
	
	public function Query($sql) {
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
}