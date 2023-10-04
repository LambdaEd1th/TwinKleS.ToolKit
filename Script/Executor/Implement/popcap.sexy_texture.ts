namespace TwinStar.Script.Executor.Implement.popcap.sexy_texture {

	// ------------------------------------------------

	// encode *
	// decode *

	export type Configuration = {
		version_number: TypicalArgumentExpression<bigint>;
		encode_compress_texture_data: TypicalArgumentExpression<boolean>;
	};

	export function injector(
		configuration: Configuration,
	): void {
		push_typical_method('popcap.sexy_texture', [
			typical_method({
				id: 'encode',
				filter: ['file', /(\.png)$/i],
				argument: [
					typical_argument_path({
						id: 'image_file',
						rule: ['file', 'input'],
						checker: null,
						automatic: null,
						condition: null,
						default: '?input',
					}),
					typical_argument_path({
						id: 'data_file',
						rule: ['file', 'output'],
						checker: null,
						automatic: (argument: { image_file: string; }) => (argument.image_file.replace(/(\.png)?$/i, '.tex')),
						condition: null,
						default: '?automatic',
					}),
					typical_argument_integer({
						id: 'version_number',
						option: KernelX.Tool.PopCap.SexyTexture.VersionNumberE,
						checker: null,
						automatic: null,
						condition: null,
						default: configuration.version_number,
					}),
					typical_argument_string({
						id: 'format',
						option: KernelX.Tool.PopCap.SexyTexture.FormatE,
						checker: null,
						automatic: null,
						condition: null,
						default: '?input',
					}),
					typical_argument_boolean({
						id: 'compress_texture_data',
						checker: null,
						automatic: null,
						condition: null,
						default: configuration.encode_compress_texture_data,
					}),
				],
				worker: ({ image_file, data_file, version_number, format, compress_texture_data }) => {
					KernelX.Tool.PopCap.SexyTexture.encode_fs(data_file, image_file, format as any, compress_texture_data, { number: version_number as any });
					return;
				},
				batch_argument: [
					typical_argument_batch({
						id: 'image_file',
						rule: 'input',
						checker: null,
						automatic: null,
						condition: null,
						default: '?input',
						item_mapper: (argument: {}, value) => (value),
					}),
					typical_argument_batch({
						id: 'data_file',
						rule: 'output',
						checker: null,
						automatic: (argument: { image_file: string; }) => (argument.image_file + '.encode'),
						condition: null,
						default: '?automatic',
						item_mapper: (argument: {}, value) => (value.replace(/(\.png)?$/i, '.tex')),
					}),
				],
				batch_worker: null,
			}),
			typical_method({
				id: 'decode',
				filter: ['file', /(\.tex)$/i],
				argument: [
					typical_argument_path({
						id: 'data_file',
						rule: ['file', 'input'],
						checker: null,
						automatic: null,
						condition: null,
						default: '?input',
					}),
					typical_argument_path({
						id: 'image_file',
						rule: ['file', 'output'],
						checker: null,
						automatic: (argument: { data_file: string; }) => (argument.data_file.replace(/(\.tex)?$/i, '.png')),
						condition: null,
						default: '?automatic',
					}),
					typical_argument_integer({
						id: 'version_number',
						option: KernelX.Tool.PopCap.SexyTexture.VersionNumberE,
						checker: null,
						automatic: null,
						condition: null,
						default: configuration.version_number,
					}),
				],
				worker: ({ data_file, image_file, version_number }) => {
					KernelX.Tool.PopCap.SexyTexture.decode_fs(data_file, image_file, { number: version_number as any });
					return;
				},
				batch_argument: [
					typical_argument_batch({
						id: 'data_file',
						rule: 'input',
						checker: null,
						automatic: null,
						condition: null,
						default: '?input',
						item_mapper: (argument: {}, value) => (value),
					}),
					typical_argument_batch({
						id: 'image_file',
						rule: 'output',
						checker: null,
						automatic: (argument: { data_file: string; }) => (argument.data_file + '.decode'),
						condition: null,
						default: '?automatic',
						item_mapper: (argument: {}, value) => (value.replace(/(\.tex)?$/i, '.png')),
					}),
				],
				batch_worker: null,
			}),
		]);
		return;
	}

	// ------------------------------------------------

}

TwinStar.Script.Executor.Implement.popcap.sexy_texture.injector;