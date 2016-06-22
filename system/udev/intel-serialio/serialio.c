// Copyright 2016 The Fuchsia Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <ddk/protocol/char.h>
#include <ddk/protocol/pci.h>
#include <ddk/binding.h>
#include <hw/pci.h>

#include <mxu/list.h>

#include <magenta/syscalls.h>
#include <magenta/types.h>

#include <stdio.h>

#include <intel-serialio/serialio.h>

static mx_status_t intel_serialio_bind(mx_driver_t* drv, mx_device_t* dev) {
    pci_protocol_t* pci;
    if (device_get_protocol(dev, MX_PROTOCOL_PCI, (void**)&pci))
        return ERR_NOT_SUPPORTED;

    const pci_config_t* pci_config;
    mx_handle_t config_handle = pci->get_config(dev, &pci_config);

    if (config_handle < 0)
        return config_handle;

    mx_status_t res;
    switch (pci_config->device_id) {
    case INTEL_BROADWELL_SERIALIO_DMA_DID:
        res = intel_serialio_bind_dma(drv, dev);
        break;
    case INTEL_BROADWELL_SERIALIO_I2C0_DID:
        res = intel_serialio_bind_i2c(drv, dev);
        break;
    case INTEL_BROADWELL_SERIALIO_I2C1_DID:
        res = intel_serialio_bind_i2c(drv, dev);
        break;
    case INTEL_BROADWELL_SERIALIO_SDIO_DID:
        res = intel_serialio_bind_sdio(drv, dev);
        break;
    case INTEL_BROADWELL_SERIALIO_SPI0_DID:
        res = intel_serialio_bind_spi(drv, dev);
        break;
    case INTEL_BROADWELL_SERIALIO_SPI1_DID:
        res = intel_serialio_bind_spi(drv, dev);
        break;
    case INTEL_BROADWELL_SERIALIO_UART0_DID:
        res = intel_serialio_bind_uart(drv, dev);
        break;
    case INTEL_BROADWELL_SERIALIO_UART1_DID:
        res = intel_serialio_bind_uart(drv, dev);
        break;
    default:
        res = ERR_NOT_SUPPORTED;
        break;
    }

    _magenta_handle_close(config_handle);
    return res;
}

static mx_bind_inst_t binding[] = {
    BI_ABORT_IF(NE, BIND_PROTOCOL, MX_PROTOCOL_PCI),
    BI_ABORT_IF(NE, BIND_PCI_VID, INTEL_VID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, INTEL_BROADWELL_SERIALIO_DMA_DID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, INTEL_BROADWELL_SERIALIO_I2C0_DID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, INTEL_BROADWELL_SERIALIO_I2C1_DID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, INTEL_BROADWELL_SERIALIO_SDIO_DID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, INTEL_BROADWELL_SERIALIO_SPI0_DID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, INTEL_BROADWELL_SERIALIO_SPI1_DID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, INTEL_BROADWELL_SERIALIO_UART0_DID),
    BI_MATCH_IF(EQ, BIND_PCI_DID, INTEL_BROADWELL_SERIALIO_UART1_DID),
};

mx_driver_t _intel_serialio BUILTIN_DRIVER = {
    .name = "intel_serialio",
    .ops = {
        .bind = intel_serialio_bind,
    },
    .binding = binding,
    .binding_size = sizeof(binding),
};