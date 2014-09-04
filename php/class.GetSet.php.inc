<?php
namespace dw;

class GetSet {
	public function __get($name) {
		$method_name = 'Get'. ucfirst($name);
		if(method_exists($this, $method_name)) {
			return $this->$method_name();
		}
	}
	public function __set($name, $value) {
		$method_name = 'Set'. ucfirst($name);
		if(method_exists($this, $method_name)) {
			return $this->$method_name($value);
		}
	}
}