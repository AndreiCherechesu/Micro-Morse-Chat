//! Network Driver
//! Commands
//!     0 -> SUCCESS
//!     1 -> Network buffer
//!     2 -> characters Networked
//!
//! Allow
//!     0 -> buffer to display
//!

// GET/POST address data_out (base64)\n
use core::cell::Cell;

use kernel::errorcode::into_statuscode;
use kernel::grant::Grant;
use kernel::hil::uart::{ReceiveClient, TransmitClient, UartData};
use kernel::process::{Error, ProcessId};
use kernel::processbuffer::{
    ReadOnlyProcessBuffer, ReadWriteProcessBuffer, ReadableProcessBuffer, WriteableProcessBuffer,
};
use kernel::syscall::{CommandReturn, SyscallDriver};
use kernel::utilities::cells::TakeCell;
use kernel::{debug, ErrorCode};
use kernel::syscall::SyscallClass::Command;

pub const DRIVER_NUM: usize = 0xa0001;
pub const METHOD_SIZE: usize = 5;

#[derive(Copy, Clone)]
enum NetworkState {
    Idle,
    Requesting(ProcessId),
}

#[derive(Default)]
pub struct AppStorage {
    address: ReadOnlyProcessBuffer,
    data_out: ReadOnlyProcessBuffer,
    data_in: ReadWriteProcessBuffer,
}

pub struct Network<'a> {
    grant_access: Grant<AppStorage, 1>,
    uart: &'a dyn UartData<'a>,
    state: Cell<NetworkState>,
    buffer: TakeCell<'static, [u8]>,
}

impl<'a> Network<'a> {
    pub fn new(
        grant_access: Grant<AppStorage, 1>,
        uart: &'a dyn UartData<'a>,
        buffer: &'static mut [u8],
    ) -> Network<'a> {
        Network {
            grant_access,
            uart: uart,
            state: Cell::new(NetworkState::Idle),
            buffer: TakeCell::new(buffer),
        }
    }
}

impl<'a> SyscallDriver for Network<'a> {
    fn command(&self,
               command_num: usize,
               r2: usize,
               _r3: usize,
               process_id: ProcessId
    ) -> CommandReturn {
        match command_num {
            0 => CommandReturn::success(),
            // send request
            // 2 => {
            1 => {
                match self.state.get() {
                    NetworkState::Idle => (),
                    _ => { return CommandReturn::failure(ErrorCode::BUSY) }
                }
                let res = self.grant_access.enter(process_id, |app_storage, _upcalls_table| {
                    let res = app_storage.address.enter(|address| {
                        let buffer = match self.buffer.take() {
                            Some(buffer) => buffer,
                            None => { return Err(ErrorCode::NOMEM); }
                        };
                        if buffer.len() < address.len() + METHOD_SIZE {
                            return Err(ErrorCode::SIZE)
                        }

                        address.copy_to_slice(
                            &mut buffer[METHOD_SIZE..address.len() + METHOD_SIZE]);
                        buffer[METHOD_SIZE + address.len()] = ' ' as u8;
                        
                        if r2 > 0 {
                            // POST
                            app_storage.data_out.enter(move |data_out| {
                                let start_idx = METHOD_SIZE + address.len() + 1;
                                let end_idx = start_idx + data_out.len();
                                if end_idx > buffer.len() + 1 {
                                    return Err(ErrorCode::INVAL)
                                }

                                data_out.copy_to_slice(&mut buffer[start_idx..end_idx]);
                                &buffer[..METHOD_SIZE].copy_from_slice("POST ".as_bytes());
                                buffer[end_idx] = '\n' as u8;

                                match self.uart.transmit_buffer(buffer, end_idx + 1) {
                                    Err((error, buffer)) => {
                                        self.buffer.replace(buffer);
                                        Err(error)
                                    }
                                    Ok(..) => {
                                        self.state.set(NetworkState::Requesting(process_id));
                                        Ok(())
                                    }
                                }
                            })
                                .map_err(|err| err.into())
                                .and_then(|x| x)
                        } else {
                            // GET
                            &buffer[..METHOD_SIZE].copy_from_slice("GET  ".as_bytes());
                            buffer[METHOD_SIZE + address.len()] = '\n' as u8;

                            match self.uart.transmit_buffer(buffer, METHOD_SIZE + address.len() + 1) {
                                Err((error, buffer)) => {
                                    self.buffer.replace(buffer);
                                    Err(error)
                                }
                                Ok(..) => {
                                    self.state.set(NetworkState::Requesting(process_id));
                                    Ok(())
                                }
                            }
                        }
                    });
                    match res {
                        Ok(Ok(())) => Ok(()),
                        Ok(Err(err)) => Err(err),
                        Err(err) => Err(err.into()),
                    }
                });
                match res {
                    Ok(Ok(())) => CommandReturn::success(),
                    Ok(Err(err)) => CommandReturn::failure(err),
                    Err(err) => CommandReturn::failure(err.into()),
                }
            }
            //     let res = self
            //         .grant_access
            //         .enter(process_id, |app_storage, _| app_storage.counter);
            //     match res {
            //         Ok(counter) => CommandReturn::success_u32(counter),
            //         Err(err) => CommandReturn::failure(err.into()),
            //     }
            // }
            _ => CommandReturn::failure(ErrorCode::NOSUPPORT),
        }
    }

    fn allow_readonly(
        &self,
        process_id: ProcessId,
        allow_num: usize,
        mut buffer: ReadOnlyProcessBuffer,
    ) -> Result<ReadOnlyProcessBuffer, (ReadOnlyProcessBuffer, ErrorCode)> {
        match allow_num {
            // address
            0 => {
                let res = self.grant_access.enter(
                    process_id,
                    |app_storage, _upcalls_table| {
                        core::mem::swap(&mut app_storage.address, &mut buffer);
                    });
                match res {
                    Ok(()) => Ok(buffer),
                    Err(err) => Err((buffer, err.into())),
                }
            }
            // data_out
            1 => {
                let res = self.grant_access.enter(
                    process_id,
                    |app_storage, _upcalls_table| {
                        core::mem::swap(&mut app_storage.data_out, &mut buffer);
                    });
                match res {
                    Ok(()) => Ok(buffer),
                    Err(err) => Err((buffer, err.into())),
                }
            }
            _ => Err((buffer, ErrorCode::NOSUPPORT)),
        }
    }

    fn allow_readwrite(
        &self,
        process_id: ProcessId,
        allow_num: usize,
        mut buffer: ReadWriteProcessBuffer,
    ) -> Result<ReadWriteProcessBuffer, (ReadWriteProcessBuffer, ErrorCode)> {
        match allow_num {
            // data_in
            0 => {
                let res = self.grant_access.enter(
                    process_id,
                    |app_storage, _upcalls_table| {
                        core::mem::swap(&mut app_storage.data_in, &mut buffer);
                    });
                match res {
                    Ok(()) => Ok(buffer),
                    Err(err) => Err((buffer, err.into())),
                }
            }
            _ => Err((buffer, ErrorCode::NOSUPPORT)),
        }
    }

    fn allocate_grant(&self, process_id: ProcessId) -> Result<(), Error> {
        self.grant_access.enter(
            process_id,
            |_app_storage, _upcalls_table| {})
    }
}

impl<'a> TransmitClient for Network<'a> {
    fn transmitted_buffer(
        &self,
        tx_buffer: &'static mut [u8],
        _tx_len: usize,
        rval: Result<(), ErrorCode>,
    ) {
        match rval {
            Ok(()) => {
                if let Err((error, buffer)) = self.uart.receive_buffer(tx_buffer, 128) {
                    self.buffer.replace(buffer);
                    if let NetworkState::Requesting(process_id) = self.state.get() {
                        let _ = self.grant_access.enter(process_id, |_, upcalls_table| {
                            let _ = upcalls_table
                                .schedule_upcall(0, (into_statuscode(Err(error)), 0, 0));
                        });
                    }
                    self.state.set(NetworkState::Idle);
                }
            }
            Err(error) => {
                self.buffer.replace(tx_buffer);
                if let NetworkState::Requesting(process_id) = self.state.get() {
                    let _ = self.grant_access.enter(process_id, |_, upcalls_table| {
                        let _ = upcalls_table
                            .schedule_upcall(0, (into_statuscode(Err(error)), 0, 0));
                    });
                }
                self.state.set(NetworkState::Idle);
            }
        }
    }
}

impl<'a> ReceiveClient for Network<'a> {
    fn received_buffer(
        &self,
        rx_buffer: &'static mut [u8],
        rx_len: usize,
        rval: Result<(), ErrorCode>,
        _error: kernel::hil::uart::Error,
    ) {
        match rval {
            Ok(()) => {
                debug!("received data");
                if let NetworkState::Requesting(process_id) = self.state.get() {
                    let _ = self.grant_access.enter(process_id, |app_storage, upcalls_table| {
                        let res = app_storage.data_in.mut_enter(|data_in| {
                            // debug!("Am intrat v1");
                            // debug!("{:?}", rx_len);
                            // debug!("{:?}", rx_buffer);
                            if rx_len < data_in.len() {
                                // debug!("Am intrat v2");
                                data_in[0..rx_len].copy_from_slice(&rx_buffer[0..rx_len]);
                                data_in[rx_len].set(0);
                            }
                            // debug!("{:?}", data_in);
                        })
                        .map_err(|err| err.into());
                        let _ = upcalls_table.schedule_upcall(0, (into_statuscode(res), rx_len, 0));
                    });
                }
            }
            Err(error) => {
                debug!("am intrat pe error");
                if let NetworkState::Requesting(process_id) = self.state.get() {
                    let _ = self.grant_access.enter(process_id, |_, upcalls_table| {
                        let _ = upcalls_table.schedule_upcall(0, (into_statuscode(Err(error)), 0, 0));
                    });
                }
            }
        }
        self.buffer.replace(rx_buffer);
        self.state.set(NetworkState::Idle);
    }
}
