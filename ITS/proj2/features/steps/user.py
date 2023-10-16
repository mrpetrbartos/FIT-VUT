from behave import *
from selenium.webdriver.common.by import By
from time import sleep


@given("the user isn't logged in")
@then("the user is no longer logged in")
def step_impl(context):
    context.driver.get(context.base_url)
    context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'float-end')]//span[contains(text(), 'My Account')]",
    ).click()
    assert context.driver.find_element(
        By.XPATH, "//a[contains(@class, 'dropdown-item') and contains(text(), 'Register')]"
    ).is_displayed()


@given("the user navigates to registration page")
def step_impl(context):
    context.driver.get(context.base_url)
    context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'float-end')]//span[contains(text(), 'My Account')]",
    ).click()
    context.driver.find_element(
        By.XPATH, "//a[contains(@class, 'dropdown-item') and contains(text(), 'Register')]"
    ).click()


@given("doesn't fill all required details")
def step_impl(context):
    pass


@when("the user clicks on Continue button")
def step_impl(context):
    btn = context.driver.find_element(
        By.XPATH,
        "//button[contains(text(), 'Continue')]",
    )
    btn.location_once_scrolled_into_view
    sleep(1)
    btn.click()
    sleep(1)
    no = len(
        context.driver.find_elements(
            By.XPATH,
            "//div[contains(@class, 'alert')]",
        )
    )
    context.already_used = no > 0


@then("the user is notified that some fields were left empty")
def step_impl(context):
    assert context.driver.find_element(
        By.XPATH, "//div[contains(@class, 'alert') and contains(., 'Warning: You must agree')]"
    ).is_displayed()


@given("fills in required details")
def step_impl(context):
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
    ).send_keys(f"pcollins@example.com")
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-password')]",
    ).send_keys("Magic123")
    inp = context.driver.find_element(
        By.XPATH,
        "//input[contains(@name, 'agree')]",
    )
    inp.location_once_scrolled_into_view
    sleep(1)
    inp.click()


@then("the user should be logged in to his new account")
def step_impl(context):
    context.driver.get(context.base_url)
    if context.already_used:
        return
    context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'float-end')]//span[contains(text(), 'My Account')]",
    ).click()
    assert context.driver.find_element(
        By.XPATH, "//a[contains(@class, 'dropdown-item') and contains(text(), 'Logout')]"
    ).is_displayed()
    context.driver.find_element(
        By.XPATH, "//a[contains(@class, 'dropdown-item') and contains(text(), 'Logout')]"
    ).click()


@given("the user navigates to Log in page")
def step_impl(context):
    context.driver.get(context.base_url)
    context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'float-end')]//span[contains(text(), 'My Account')]",
    ).click()
    context.driver.find_element(
        By.XPATH, "//a[contains(@class, 'dropdown-item') and contains(text(), 'Login')]"
    ).click()


@given("fills in wrong details")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-email')]",
    ).send_keys("thisaccshoud@not.exist")
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-password')]",
    ).send_keys("pass12345")


@when("the user clicks on Login button")
def step_impl(context):
    btn = context.driver.find_element(
        By.XPATH,
        "//button[contains(text(), 'Login')]",
    )
    btn.click()
    sleep(1)


@then("the user should be notified that the account doesn't exist")
def step_impl(context):
    assert context.driver.find_element(
        By.XPATH, "//div[contains(@class, 'alert') and contains(., 'Warning: No match')]"
    ).is_displayed()


@given("fills in required login details")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-email')]",
    ).send_keys(f"pcollins@example.com")
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-password')]",
    ).send_keys("Magic123")


@then("the user should be logged in to his account")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'float-end')]//span[contains(text(), 'My Account')]",
    ).click()
    assert context.driver.find_element(
        By.XPATH, "//a[contains(@class, 'dropdown-item') and contains(text(), 'Logout')]"
    ).is_displayed()


@given("the user is logged in")
def step_impl(context):
    context.driver.get(context.base_url)
    context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'float-end')]//span[contains(text(), 'My Account')]",
    ).click()
    no = len(
        context.driver.find_elements(By.XPATH, "//a[contains(@class, 'dropdown-item') and contains(text(), 'Logout')]")
    )
    if no == 0:
        context.driver.find_element(
            By.XPATH,
            "//div[contains(@class, 'float-end')]//span[contains(text(), 'My Account')]",
        ).click()
        context.driver.find_element(
            By.XPATH, "//a[contains(@class, 'dropdown-item') and contains(text(), 'Login')]"
        ).click()
        context.driver.find_element(
            By.XPATH,
            "//input[contains(@id, 'input-email')]",
        ).send_keys(f"pcollins@example.com")
        context.driver.find_element(
            By.XPATH,
            "//input[contains(@id, 'input-password')]",
        ).send_keys("Magic123")
        context.driver.find_element(
            By.XPATH,
            "//button[contains(text(), 'Login')]",
        ).click()


@when("the user clicks on Logout in My Account section")
def step_impl(context):
    context.driver.get(context.base_url)
    context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'float-end')]//span[contains(text(), 'My Account')]",
    ).click()
    context.driver.find_element(
        By.XPATH, "//a[contains(@class, 'dropdown-item') and contains(text(), 'Logout')]"
    ).click()


@given("the user navigates to Change password")
def step_impl(context):
    context.driver.get(context.base_url)
    context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'float-end')]//span[contains(text(), 'My Account')]",
    ).click()
    context.driver.find_element(
        By.XPATH, "//a[contains(@class, 'dropdown-item') and contains(text(), 'My Account')]"
    ).click()
    context.driver.find_element(By.XPATH, "//a[contains(text(), 'Change your password')]").click()


@given("the new password is filled in")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-confirm')]",
    ).send_keys(f"Magic123")
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-password')]",
    ).send_keys("Magic123")


@then("success notification should be shown")
def step_impl(context):
    assert context.driver.find_element(
        By.XPATH,
        "//div[contains(@class, 'alert') and contains(., 'Success: Your password has been')]",
    ).is_displayed()
