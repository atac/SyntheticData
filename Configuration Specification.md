Configuration files for SynthCh10Gen provide the application with data source and channel setup information for a single generation run. These files (see [examples](#example-configuration)) are in JSON format and have the following requirements.


## Configuration File Description

|  Key  | Type  | Required | Description | Default |
| ------------ | ------------ | ------------ | ------------ | ------------ |
| `programName` | `string`  | | Sets the G\PN TMATS attribute | "Synthetic Chapter 10" |
| `outputDirectory` | `string` | Yes | Output location of the generated Chapter 10 data file | |
| `outputFilename` | `string` | | Name of the generated Chapter 10 data file | "synthetic_data_yyyymmdd_hhmmss.ch10" |
| `timeBasis` | `string` | | Name of the channel from which to derive simulation clock time | The first channel defined in the channels array |
| `startTime` | `string` | | The [Timestamp](#timestamps) of a row in the time basis data source from where the system should begin processing data. Data before this timestamp is ignored in all aligned data sources. <br><br> *Note: Start time is determined prior to source [time shifting](#source-description). Combining a basis timeShift and startTime may produce unintended results.* | The first row of the time basis data source |
| `sources` | `object` | | Contains single-definition sources as an alternative to defining a [Source](#source-description) within each [Channel](#channel-description) object | |
| `channels` | `array` | Yes | Contains one or more [Channel](#channel-description) objects | |
| `mappings` | `object` | | Name mappings used for associating source columns with fields in built-in output formats | |

### Channel Description

|  Key  | Type  | Required | Description | Default |
| ------------ | ------------ | ------------ | ------------ | ------------ |
| `id` | `uint` | | Channel ID, sets the R-x\TK1-n TMATS attribute | Starts at 2 and increments by 1 for each unspecified ID |
| `name` | `string` | | Channel name, sets the R-x\DSI-n TMATS attribute | Auto generated name |
| `type` | `string` | Yes | Channel [Type](#channel-types), sets the R-x\CDT-n TMATS attribute | |
| `format` | `string` | | Data packing [format](#channel-data-formats) | Unformatted
| `source` | `object` / `string` | Yes | A data source which feeds this channel. Allows either the object definition or the key of a [Source](#source-description) object defined in the [top-level](#configuration-file-description) sources object |
| `pollRate` | `object` | | Sets the [rate](#rate-description) at which the source data is polled | 50 Hz |
| `packetRate` | `object` | | Sets the [rate](#rate-description) at which packets are published to the output stream | 10 Hz |

### Source Description
Sources are data files containing timestamped telemetry or other flight data. This data is converted to the Chapter 10 format specified by any referencing channel. When multiple sources are defined, start times are aligned to coincide with the start time of the [`timeBasis`](#configuration-file-description) channel. Use the `timeShift` property to shift times left or right on the data timeline relative to the basis.

The left-most column of any text-based data source is always expected to contain a [Timestamp](#timestamps). SQL data sources should have a RowNum index column followed by a timestamp column.

|  Key  | Type  | Required | Description | Default |
| ------------ | ------------ | ------------ | ------------ | ------------ |
| `pathname` | `string` | Yes | Pathname of a data file used as the source for packet generation. Additional properties may be required depending on the source file type. (See below) | |
| `mapping` | `string` | | Name of a field name mapping set from the [mappings](#mappings-description) property | No mapping |
| `timeShift` | `float` | | Value (in seconds) to shift the source for data alignment. Positive values shift forward relative to the basis (right on timeline), and negative values shift backward relative to the basis (left on timeline). <br><br> *Note: Shifting the basis (not recommended) will only shift that source; other sources are shifted relative to the origin of the unshifted basis.* | No bias |

##### SQLite Database (.sql)
|  Key  | Type  | Required | Description | Default |
|-|-|-|-|-|
| `table` | `string` | Yes | Name of the DB table containing the desired source data columns |

##### Comma-Separated Value (.csv)
|  Key  | Type  | Required | Description | Default |
|-|-|-|-|-|
|-|-|-|-|-|

##### Tab-Separated Value (.txt, .tsv)
|  Key  | Type  | Required | Description | Default |
|-|-|-|-|-|
|-|-|-|-|-|

### Rate Description

|  Key  | Type  | Required | Description | Default |
| ------------ | ------------ | ------------ | ------------ | ------------ |
| `value` | `uint` | | Numerical rate value | 0, corresponds to unsampled or data-triggered |
| `unit` | `string` | | Rate Units | default milliseconds |

### Channel Types
| TMATS Channel Type | Allowed Values (case-insensitive) |
| ---- | ---- |
| PCMIN | pcmin, pcm |
| VIDIN | vidin, video, vid |
| 1553IN | 1553in, 1553, ms1553, mil_std_1553, mil-std-1553 |
| 429IN | 429in, a429, arinc429, arinc-429, arinc_429 |

### Channel Data Formats
| Format | Allowed Values (case-insensitive) | Description |
|-|-|-|
| Unformatted | unformatted | Unformatted data does no special formatting or packing for packet data. (e.g. one PCM word per field value) |
| ~~Custom~~ | ~~custom~~ | **Unimplemented**. Allows for a custom packing definition by defining start words, start bits, and lengths for each field. |
| Synthetic Format 1 | synthformat1, synthfmt1 | The original packing format definied in the Synthetic Data Generator ICD |

### Rate Units
| Unit | Allowed Values (case-insensitive) |
| ---- | ---- |
| seconds | seconds, sec, s |
| milliseconds | milliseconds, milli, ms |
| microseconds | microseconds, micro, us |
| rtc (ticks) | rtc |
| nanoseconds | nanoseconds, nano, ns |
| hertz | hertz, hz, frequency |

### Mappings Description

The mappings object contains a set of field-name-mapping key/value pairs where the **value** is the expected data source field name, and the **key** is the field name in the generated output file (i.e. the field name is mapped from value to key). This allows generating data formats from a variety of sources by normalizing field names to match a format specification such as an ICD. Omitting an output field key or providing an empty string value will cause no mapping to be applied to that field.

| Key | Type | Required | Description |
|-|-|-|-|
| `<mapName>` | `object` | | A set of field name mapping key/value pairs |

```
mappings : {
	"navigationMap" : {
		"LATITUDE" : "AC_LAT",
		"LONGITUDE" : "AC_LON",
		"ALTITUDE" : "AC_ALT"
	},
	"systemMap" : {
		"RUDDER" : "RUDD",
		"FLAPS" : "FLAP",
		"LANDING GEAR DOWN" : "LGDN"
	}
}
```

## Timestamps
Supported timestamp formats are shown in the following table. Any references to timestamp fields in this document imply required adherance to one of these formats.

| Format | Description | Example |
|-|-|-|
| seconds | Float value with seconds units | 1234.56 |
| mm:ss | String value for minutes and seconds (float) | 12:34.56
| hh:mm:ss | String value for hours, minutes, and seconds (float) | 12:34:56.78 |
| ddd hh:mm:ss <br> ddd:hh:mm:ss | String value for days (DoY), hours, minutes, and seconds (float) | 012:12:34:56.78 |
| MM-dd hh:mm:ss <br> MM dd hh:mm:ss <br> MM:dd:hh:mm:ss | String value for months, days (DoM), hours, minutes, and seconds (float) | 01-23 12:34:56.78 |
| yyyy-ddd hh:mm:ss <br> yyyy ddd hh:mm:ss <br> yyyy:ddd:hh:mm:ss | String value for year, days (DoY), hours, minutes, seconds (float) | 2031-012 12:34:56.78 |
| yyyy-MM-dd hh:mm:ss <br> yyyy MM dd hh:mm:ss <br> yyyy:MM:dd:hh:mm:ss | String value for year, months, days (DoM), hours, minutes, seconds (float) | 2031-01-02 12:34:56.78 |

##  Example Configuration

```
{
	"programName" : "Sample Data Generation",
	"outputDirectory" : "C:/data/output/",
	"outputFilename" : "SampleDataGen.ch10",
	"timeBasis" : "PCMin20",
	"startTime" : 120.00,
	
	"sources" : {
		"flight42" : {
			"pathname" : "C:/data/sources/flight42.csv",
			"mapping" : "navigationMap"
		}
	}

	"channels" : [
		{
			"name" : "PCMin20",
			"id" : 20,
			"type" : "pcm",
			"source" : "flight42"
			"pollRate" : {
				"value" : 100,
				"unit" : "hz"
			},
			"packetRate" : {
				"value" : 20,
				"unit" : "hz"
			}
		}
	],

	mappings : {
		"navigationMap" : {
			"LAT" : "AC_LAT",
			"LON" : "AC_LON",
			"ALT" : "AC_ALT"
		}
	}
}
```

##  Example Minimal Configuration
```
{	
    "outputDirectory" : "C:/data/output",
	"channels" : [
		{
			"type" : "pcm",
			"source" : {
				"pathname" : "C:/atac/vsprojects/SyntheticData/Debug/30931-small.csv"
			}
		}
	]
}
```
