from behave import *
from time import sleep
from selenium.webdriver.common.by import By


@given("a web browser is at a product page")
def step_impl(context):
    context.driver.get(context.base_url)
    context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'product-thumb')]",
    ).click()


@given("the user selects required options")
def step_impl(context):
    search = context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-quantity')]",
    )
    search.clear()
    search.send_keys("1")


@when("the user clicks on Add to Cart button")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//button[contains(@id, 'button-cart')]",
    ).click()


@then("the subtotal is updated and notification is displayed")
def step_impl(context):
    assert context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'alert') and contains(text(), 'Success: You have added')]",
    ).is_displayed()


@given("the user navigates to shopping cart")
def step_impl(context):
    context.driver.get(context.base_url)
    context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'float-end')]//span[contains(text(), 'Shopping Cart')]",
    ).click()


@given("the Shopping Cart isn't empty")
def step_impl(context):
    cart = context.driver.find_element(
        By.XPATH,
        "//div[contains(@id, 'header-cart')]//button",
    ).text
    if "0 item" in cart:
        context.driver.get(context.base_url)
        context.driver.find_element(
            By.XPATH,
            "//div[contains(@class, 'product-thumb') and contains(., 'iPhone')]",
        ).click()
        context.driver.find_element(
            By.XPATH,
            "//button[contains(@id, 'button-cart')]",
        ).click()


@when("the user updates quantity for a product")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//i[contains(@class, 'fa-rotate')]/ancestor::button",
    ).click()


@then("the subtotal is updated")
def step_impl(context):
    assert context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'alert') and contains(text(), 'Success: You have modified')]",
    ).is_displayed()


@when("the user removes a product from his shopping cart")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//i[contains(@class, 'fa-rotate')]/ancestor::button/following-sibling::button[1]",
    ).click()


@then("the subtotal is updated and product removed")
def step_impl(context):
    sleep(1.5)
    assert (
        "0 item"
        in context.driver.find_element(
            By.XPATH,
            "//div[contains(@id, 'header-cart')]//button",
        ).text
    )


@given("the user navigates to Checkout")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'float-end')]//span[contains(text(), 'Checkout')]",
    ).click()


@given("fills in the required details")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-guest')]",
    ).click()
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-firstname')]",
    ).send_keys("Philip")
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-lastname')]",
    ).send_keys("Collins")
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-email')]",
    ).send_keys("pcollins@example.com")
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-shipping-address-1')]",
    ).send_keys("221B Baker Street")
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-shipping-city')]",
    ).send_keys("London")
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-shipping-postcode')]",
    ).send_keys("NW1")
    continue_btn = context.driver.find_element(
        By.XPATH,
        "//button[contains(@id, 'button-register')]",
    )
    continue_btn.location_once_scrolled_into_view
    sleep(1)
    context.driver.find_element(
        By.XPATH,
        "//select[contains(@id, 'input-shipping-country')]",
    ).click()
    context.driver.find_element(
        By.XPATH,
        "//option[contains(text(), 'United Kingdom')]",
    ).click()
    context.driver.find_element(
        By.XPATH,
        "//select[contains(@id, 'input-shipping-zone')]",
    ).click()
    context.driver.find_element(
        By.XPATH,
        "//option[contains(text(), 'Greater London')]",
    ).click()
    continue_btn.click()
    shipping_method = context.driver.find_element(
        By.XPATH,
        "//select[contains(@id, 'input-shipping-method')]",
    )
    shipping_method.location_once_scrolled_into_view
    sleep(1)
    shipping_method.click()
    context.driver.find_element(
        By.XPATH,
        "//option[contains(text(), 'Flat Shipping Rate')]",
    ).click()
    context.driver.find_element(
        By.XPATH,
        "//select[contains(@id, 'input-payment-method')]",
    ).click()
    context.driver.find_element(
        By.XPATH,
        "//option[contains(text(), 'Cash On Delivery')]",
    ).click()


@when("the user clicks on Confirm Order button")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//button[contains(@id, 'button-confirm')]",
    ).click()


@then("the successful order page should be shown")
def step_impl(context):
    assert context.driver.find_element(
        By.XPATH, "//h1[contains(text(), 'Your order has been placed!')]"
    ).is_displayed()
