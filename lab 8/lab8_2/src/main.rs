use esp_idf_hal::{delay::FreeRtos, gpio:: PinDriver, peripherals::Peripherals};

fn main() {
    esp_idf_svc::sys::link_patches();

	let peripherals = Peripherals::take().unwrap();
	let mut led_pin = PinDriver::output(peripherals.pins.gpio7).unwrap();

	loop {
		led_pin.set_low().unwrap();
		println!("led on.");
		FreeRtos::delay_ms(1000);

		led_pin.set_high().unwrap();
		println!("led off.");
		FreeRtos::delay_ms(1000);	
	}
}
