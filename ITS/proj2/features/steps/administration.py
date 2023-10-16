from behave import *
from selenium.webdriver.common.by import By
from time import sleep
from random import random


@given('the user appends "/administration" to the url')
@given("a web browser is at the admin login page")
def step_impl(context):
    context.driver.get(context.base_url + "/administration")


@when("the user tries to access the admin site")
def step_impl(context):
    pass


@then("administration page should be displayed")
@given("the administrator isn't logged in")
def step_impl(context):
    assert context.driver.find_element(
        By.XPATH, "//div[contains(@class, 'card-header') and contains(., 'Please enter your login')]"
    ).is_displayed()


@given("the user fills in his credentials")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-username')]",
    ).send_keys(f"user")
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-password')]",
    ).send_keys("bitnami")


@then("the user should be logged in to admin dashboard")
@given("administrator is logged in")
def step_impl(context):
    context.driver.find_element(By.XPATH, "//li[contains(@id, 'menu-dashboard')]").click()
    assert context.driver.find_element(By.XPATH, "//h1[contains(text(), 'Dashboard')]").is_displayed()


@given("the user navigates to Categories page")
def step_impl(context):
    context.driver.find_element(By.XPATH, "//li[contains(@id, 'menu-catalog')]").click()
    sleep(1)
    context.driver.find_element(By.XPATH, "//a[contains(., 'Categories')]").click()


@given("the user clicks on Add New button")
def step_impl(context):
    sleep(1)
    context.driver.find_element(
        By.XPATH,
        "//i[contains(@class, 'fa-plus')]/ancestor::a",
    ).click()


@when("the user clicks on modification Save button")
def step_impl(context):
    sleep(1)
    context.driver.find_element(
        By.XPATH,
        "//i[contains(@class, 'fa-floppy-disk')]/ancestor::button",
    ).click()


@given("the user fills in all the required details")
def step_impl(context):
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-name-1')]",
    ).send_keys("Mrkev")
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-meta-title-1')]",
    ).send_keys("mrkvicka")
    context.driver.find_element(
        By.XPATH,
        "//a[contains(text(), 'SEO') and contains(@class, 'nav-link')]",
    ).click()
    context.driver.find_element(
        By.XPATH,
        "//input[contains(@id, 'input-keyword-0-1')]",
    ).send_keys(f"mrkvicka{int(random()*1000)}")


@then("success modification notification should be shown")
def step_impl(context):
    assert context.driver.find_element(
        By.XPATH, "//div[contains(@class, 'alert') and contains(., 'Success: You have modified')]"
    ).is_displayed()


@given("the user navigates to details of a product")
def step_impl(context):
    context.driver.find_element(By.XPATH, "//li[contains(@id, 'menu-catalog')]").click()
    sleep(1)
    context.driver.find_element(By.XPATH, "//a[contains(., 'Products')]").click()
    sleep(1)
    context.driver.find_element(
        By.XPATH,
        "//i[contains(@class, 'fa-pencil')]/ancestor::a",
    ).click()


@given("the user modifies saved values")
def step_impl(context):
    pass
