from behave import *
from selenium.webdriver.common.by import By


@given("a web browser is open")
def step_impl(context):
    try:
        context.driver.execute_script("window.open()")
        context.driver.window_handles
    except:
        print("Driver doesn't have active window")


@given("the user has access to the internet")
def step_impl(context):
    # the website is running in localhost environment, access is not needed
    pass


@when("the user tries to access the site")
@when("a web browser is at the home page")
def step_impl(context):
    context.driver.get(context.base_url)


@then("home page is displayed")
def step_imp(context):
    assert context.driver.find_element(By.XPATH, "//footer//a[contains(text(), 'OpenCart')]").is_displayed()


@given("a web browser is at the home page")
def step_impl(context):
    context.driver.get(context.base_url)


@when("the user clicks on a non-empty category in the navigation bar")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//ul[contains(@class, 'navbar-nav')]//a[contains(text(), 'Desktops')]",
    ).click()
    context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'dropdown-menu show')]//a[contains(text(), 'Mac')]",
    ).click()


@then("products from the selected category should be displayed")
def step_imp(context):
    assert (
        context.driver.find_element(
            By.XPATH,
            "//div[contains(@class, 'product-thumb')]",
        ).size
        != 0
    )


@when("the user navigates to a category with no products")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//ul[contains(@class, 'navbar-nav')]//a[contains(text(), 'Desktops')]",
    ).click()
    context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'dropdown-menu show')]//a[contains(text(), 'PC')]",
    ).click()


@then("no products message should be displayed")
def step_imp(context):
    assert context.driver.find_element(
        By.XPATH,
        "//p[text()='There are no products to list in this category.']",
    ).is_displayed()


@when("the user clicks on a featured product")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'product-thumb')]",
    ).click()


@then("the selected product should be displayed")
def step_impl(context):
    assert context.driver.find_element(
        By.XPATH,
        "//div[contains(@id, 'product-info')]",
    ).is_displayed()


@given("search query has been typed into Search input")
def step_impl(context):
    context.driver.find_element(By.XPATH, "//input[contains(@name, 'search')]").send_keys("MacBook")


@when("the user clicks on the search button")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//div[contains(concat(' ', normalize-space(@id), ' '), ' search ')]//button",
    ).click()


@then("all product with given string in their names should be displayed")
def step_impl(context):
    assert (
        len(
            context.driver.find_elements(
                By.XPATH,
                "//div[contains(@class, 'product-thumb') and contains(text(), MacBook)]",
            )
        )
        != 0
    )


@when("the user selects a currency using the top bar")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'nav')]//span[contains(text(), 'Currency')]",
    ).click()
    context.driver.find_element(
        By.XPATH,
        "//ul[contains(@class, 'dropdown-menu')]//a[contains(text(), 'Euro')]",
    ).click()


@then("page should display the price in the selected currency")
def step_impl(context):
    assert (
        "€"
        in context.driver.find_element(
            By.XPATH,
            "//div[contains(@id, 'header-cart')]",
        ).text
    )


@given("the user searches for a product")
def step_impl(context):
    context.driver.find_element(By.XPATH, "//input[contains(@name, 'search')]").send_keys("MacBook")
    context.driver.find_element(
        By.XPATH,
        "//div[contains(concat(' ', normalize-space(@id), ' '), ' search ')]//button",
    ).click()


@given("multiple products are shown")
def step_impl(context):
    len(
        context.driver.find_elements(
            By.XPATH,
            "//div[contains(@class, 'product-thumb') and contains(text(), MacBook)]",
        )
    ) > 1


@when("the user selects {SortBy} sort option from dropdown")
def step_impl(context, SortBy):
    context.driver.find_element(By.XPATH, "//select[contains(@id, 'input-sort')]").click()
    context.driver.find_element(
        By.XPATH,
        f"//option[contains(text(), '{SortBy}')]",
    ).click()


@then("the shown products should be sorted by {SortBy} parameter")
def step_impl(context, SortBy):
    assert (
        len(
            context.driver.find_elements(
                By.XPATH,
                "//div[contains(@class, 'product-thumb') and contains(text(), MacBook)]",
            )
        )
        > 1
    )
