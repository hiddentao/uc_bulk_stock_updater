<?php
// $Id$
/*
Bulk stock updater module for Drupal 
Copyright (C) 2010 Ramesh Nair (www.hiddentao.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/


/**
 * Main admin page.
 * @return HTML.
 */
function uc_bulk_stock_updater_stock_overview() {
	drupal_goto(_UC_BULK_STOCK_UPDATER_MENU_UPDATE);
	return;
}

/**
 * Stock levels editing page.
 * @return HTML.
 */
function uc_bulk_stock_updater_stock_update() {
  drupal_add_css(drupal_get_path('module', 'uc_stock') . '/uc_stock.css');
  drupal_add_css(drupal_get_path('module', 'uc_bulk_stock_updater') . '/uc_bulk_stock_updater.css');
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

  $sql = "SELECT s.nid, sku, title, stock, threshold FROM {uc_product_stock} as s LEFT JOIN {node} as n ON s.nid = n.nid WHERE active = 1 AND title <> ''";

  $result = pager_query($sql . tablesort_sql($header), $page_size, 0, NULL);
  
  while ($stock = db_fetch_object($result)) {
  	
  	$stock->sku = check_plain($stock->sku);
  	$stock->title = check_plain($stock->title);
  	
  	// for filtering purposes
  	$filter_val = strtolower($stock->sku . $stock->title . $stock->stock . $stock->threshold);
    // Add the data to a table row for display.
    $rows[] = array(
      'data' => array(
        array('data' => "<span id=\"$filter_val\">$stock->sku</span>"),
        array('data' => l($stock->title, 'node/' . $stock->nid)),
        array('data' => "<input size=\"3\" maxlength=\"9\" class=\"uc_bulk_stock_updater-value\" name=\"$stock->sku\" value=\"$stock->stock\" />"),
        array('data' => $stock->threshold),
      ),
      'class' => (intval($stock->threshold) >= intval($stock->stock)) ? 'uc-stock-below-threshold' : 'uc-stock-above-threshold',
    );
  }

  $output = '';
  
  // add filter field
  if (!is_null($_GET['nopage'])) {
  	$output .= '<p><label for="uc_bulk_stock_updater_filter">Filter:</label><input id="uc_bulk_stock_updater_filter" size="15" /></p>';
  }
  
  $output .= theme('table', $header, $rows, array('width' => '100%', 'class' => 'uc-stock-table'))
          . theme_pager(NULL, $page_size);
  $output .= '<div class="uc-reports-links">' . ((!is_null($_GET['nopage'])) ? l(t('Show paged records'), _UC_BULK_STOCK_UPDATER_MENU_UPDATE) : l(t('Show all records'), _UC_BULK_STOCK_UPDATER_MENU_UPDATE, $options = array('query' => 'nopage=1'))) . '</div>';
  
  return $output;
}

/** 
 * AJAX callback.
 */
function uc_bulk_stock_updater_stock_update_ajax() {
	if (!isset($_POST['sku']) || !isset($_POST['stock']) || 0 > intval($_POST['stock'])) {
		$err = t('Invalid parameters supplied');
	} else {
  		if (!db_query("UPDATE {uc_product_stock} SET stock = %d WHERE sku = '%s'", $_POST['stock'], $_POST['sku']))
			$err = t('Error saving to db');
	}
	
	$ret = array();
	
	if (isset($err))
		$ret['error'] = $err;
		
	drupal_json($ret);
	exit();
}




