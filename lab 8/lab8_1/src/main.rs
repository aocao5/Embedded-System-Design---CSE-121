use std::thread::sleep;
use std::time::{Duration, Instant};

fn main() {
	let interval = Duration::from_secs(2);
	let mut next_time = Instant::now() + interval;

	loop {
    	println!("Hello, world!");
		println!("Amos Cao\n");
	
		sleep(next_time - Instant::now());
		next_time += interval;
	}
}
