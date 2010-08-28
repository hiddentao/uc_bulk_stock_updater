<?php
// $Id$

/**
 * @file
 * The Ubercart stock updater module.
 *
 * Contains the bulk of the functionality.
 */


/**
 * Stock levels editing page.
 * @return HTML.
 */
function uc_bulk_stock_updater_stock_update() {
  drupal_add_css(drupal_get_path('module', 'uc_stock') . '/uc_stock.css');
  drupal_add_css(drupal_get_path('module', 'uc_bulk_stock_updater') . '/uc_bulk_stock_updater.css');
  drupal_add_js('var g_uc_bulk_stock_updater_ajax_url = "' . url(_UC_BULK_STOCK_UPDATER_MENU_UPDATE_AJAX) . '";', 'inline');
  drupal_add_js(drupal_get_path('module', 'uc_bulk_stock_updater') . '/uc_bulk_stock_updater.js');
  
  $page_size = (!is_null($_GET['nopage'])) ? UC_REPORTS_MAX_RECORDS : variable_get('uc_reports_table_size', 30);
  $rows = array();

  $header = array(
    array('data' => t('SKU'), 'field' => 'sku', 'sort' => 'asc'),
    array('data' => t('Product'), 'field' => 'title'),
    array('data' => t('Stock'), 'field' => 'stock'),
    array('data' => t('Threshold'), 'field' => 'threshold'),
  );

  $csv_rows[] = array(t('SKU'), t('Product'), t('Stock'), t('Threshold'));

  $sql = "SELECT s.nid, sku, title, stock, threshold FROM {uc_product_stock} AS s LEFT JOIN {node} AS n ON s.nid = n.nid WHERE active = 1 AND title <> ''";

  $result = pager_query($sql . tablesort_sql($header), $page_size, 0, NULL);
  
  while ($stock = db_fetch_object($result)) {
  	
  	$stock->sku = check_plain($stock->sku);
  	$stock->title = check_plain($stock->title);
  	
  	// for filtering purposes
  	$filter_val = implode('__',array_map('drupal_strtolower',array($stock->sku, $stock->title, $stock->stock, $stock->threshold)));
  	
  	// construct input using FAPI
  	$stock_textfield = theme_textfield(array(
  	  '#name' => $stock->sku,
  	  '#value' => $stock->stock,
  	  '#size' => 3,
  	  '#maxlength' => 9,
  	  '#attributes' => array('class' => 'uc_bulk_stock_updater_value'),
  	  '#parents' => array(),
  	));
  	
    // Add the data to a table row for display.
    $rows[] = array(
      'data' => array(
        array('data' => "<span id='$filter_val'>$stock->sku</span>"),
        array('data' => l($stock->title, 'node/' . $stock->nid)),
        array('data' => $stock_textfield),
        array('data' => $stock->threshold),
      ),
      'class' => (intval($stock->threshold) >= intval($stock->stock)) ? 'uc-stock-below-threshold' : 'uc-stock-above-threshold',
    );
  }

  $output = '';
  
  $link = '<div class="uc-reports-links">' 
               . ((!is_null($_GET['nopage'])) ? l(t('Show paged records'), _UC_BULK_STOCK_UPDATER_MENU_UPDATE) : l(t('Show all records'), _UC_BULK_STOCK_UPDATER_MENU_UPDATE, $options = array('query' => 'nopage=1'))) 
               . '</div>';
  
  $output .= $link;
               
  // add filter field
  if (!is_null($_GET['nopage'])) {
    $output .= theme_textfield(array(
      '#title' => 'Filter',
      '#id' => 'uc_bulk_stock_updater_filter',
      '#size' => 15,
      '#parents' => array(),
    ));
  }

  $output .= theme('table', $header, $rows, array('width' => '100%', 'class' => 'uc-stock-table'))
          . theme_pager(NULL, $page_size);
          
  $output .= $link;
  
  return $output;
}




/** 
 * AJAX callback.
 */
function uc_bulk_stock_updater_stock_update_ajax() {
	if (!isset($_POST['sku']) || !isset($_POST['stock']) || 0 > intval($_POST['stock'])) {
		$err = t('Invalid parameters supplied');
	} 
	else {
  		if (!db_query("UPDATE {uc_product_stock} SET stock = %d WHERE sku = '%s'", $_POST['stock'], $_POST['sku']))
			$err = t('Error saving to db');
	}
	
	$ret = array();
	
	if (isset($err))
		$ret['error'] = $err;
		
	drupal_json($ret);
	exit();
}




