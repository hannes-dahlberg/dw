function Dw () {
	this.ajax_before_request = [];
	this.ajax_before_response = [];
	this.ajax_after_response = [];
	
	this.submit_ajax = function(element) {
		
		form = '';
		if(element.is('form')) {
			form = element;
		} else if(this.isset(element.attr('data-dw-target-form'))) {
			form = $('#' + element.attr('data-dw-target-form'));
		}
		
		url = '';
		target_ids = '';
		form_values = {};
		form_method = 'get';
		
		if(this.isset(element.attr('data-dw-method')) && element.attr('data-dw-method') == 'post') {
			form_method = 'post';
		}
		//Submitting form
		if(form != '') {
			//Form submit url
			if(this.isset(form.attr('action'))) {
				url = form.attr('action');
			}
			
			//Getting the target ID's
			if(this.isset(form.find('#dw_target_ids').val())) { //Looking for input with ID
				target_ids = form.find('#dw_target_ids').val();
			} else if(this.isset(form.find('input[name=dw_target_ids]').val())) { //Looking for input with name
				target_ids = form.find('input[name=dw_target_ids]').val();
			}
			
			//Getting form values
			$.each(form.serializeArray(), function(i, field) {
				if(field.name != 'dw_target_ids') {
					form_values[field.name] = field.value;	
				}
			});
			
			//Form method
			if(this.isset(form.attr('method'))) {
				if(form.attr('method') == 'post') {
					form_method = 'post';
				} else if(form.attr('method') == 'get') {
					form_method = 'post';	
				}
			}
		}
		
		//Get url from href or data action
		if(url == '' && this.isset(element.attr('href'))) {
			url = element.attr('href');
		} else if(url == '' && this.isset(element.attr('data-dw-action'))) {
			url = element.attr('data-dw-action');
		} else {
			url = '/index.php';
		}
		
		
		
		//Getting target ID's from clicked element if not set in form
		if(target_ids == '' && this.isset(element.attr('data-dw-target-ids'))) {
			target_ids = element.attr('data-dw-target-ids');
		}
		
		if(target_ids != '') {
			target_ids = target_ids.split(',');
		}
		
		//Calling functions for elements
		for(a = 0; a < target_ids.length; a++) {
			if(typeof this.ajax_before_request[target_ids[a]] == 'function') {
				this.ajax_before_request[target_ids[a]](url, form_values);
			}
		}
		
		//Make the AJAX call
		$.ajax({
			url: url,
			type: form_method,
			data: form_values,
			dataType: 'html',
			success: function(data, text_status, jqxhr) {
				response = $(data);
				
				//Call functions, add data to DOM and call after-functions
				for(a = 0; a < target_ids.length; a++) {
					if(typeof dw.ajax_before_response[target_ids[a]] == 'function') {
						dw.ajax_before_response[target_ids[a]](url, form_values, response);
					}
					if(typeof $('#' + target_ids[a]) != 'undefined') {
						$('#' + target_ids[a]).html(response.find('#' + target_ids[a]).html());	
					}
					if(typeof dw.ajax_after_response[target_ids[a]] == 'function') {
						dw.ajax_after_response[target_ids[a]](url, form_values, response);
					}
				}
			},
			error: function(jqxhr, text_status, error_thrown) {
				
			}
		});
	}

	this.isset = function(object) {
		if(typeof object != 'undefined') {
			return true;
		}
		
		return false;
	}
}
var dw = new Dw();

$(function() {
	$(document).on('submit', 'form.dw-ajax', function(e) {
		e.preventDefault();
		dw.submit_ajax($(this));
	});
	$(document).on('click', 'a.dw-ajax, span.dw-ajax, label.dw-ajax', function(e) {
		if($(this).is('a')) {
			e.preventDefault();
		}
		dw.submit_ajax($(this));
	});
});